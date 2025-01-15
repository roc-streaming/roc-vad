/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "device.hpp"
#include "build_info.hpp"
#include "receiver.hpp"
#include "sender.hpp"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <cassert>
#include <stdexcept>

namespace rocvad {

namespace {


size_t compute_channel_count(roc_channel_layout channel_layout)
{
    switch (channel_layout) {
    case ROC_CHANNEL_LAYOUT_MONO:
        return 1;

    case ROC_CHANNEL_LAYOUT_STEREO:
        return 2;

    default:
        throw std::invalid_argument("unsupported channel_layout");
    }
}

size_t compute_buffer_samples(int64_t buffer_length_ns, size_t sample_rate)
{
    if (buffer_length_ns < 0) {
        throw std::invalid_argument("negative buffer_length");
    }

    if (buffer_length_ns == 0) {
        throw std::invalid_argument("zero buffer_length");
    }

    return (size_t)std::round(buffer_length_ns / 1'000'000'000.0 * sample_rate);
}

aspl::DeviceParameters make_device_params(const DeviceInfo& info)
{
    aspl::DeviceParameters device_params;

    device_params.Name = info.name;
    device_params.Manufacturer = BuildInfo::driver_manufacturer;
    device_params.DeviceUID = info.uid;
    device_params.ModelUID = BuildInfo::driver_bundle_id;

    device_params.CanBeDefault = true;
    device_params.CanBeDefaultForSystemSounds = true;

    device_params.EnableMixing = true;
    device_params.EnableRealtimeTracing = false;

    device_params.SampleRate = info.device_encoding.sample_rate;
    device_params.ChannelCount = info.device_encoding.channel_count;

    device_params.ZeroTimeStampPeriod = info.device_encoding.buffer_samples;

    spdlog::debug(
        "device parameters: Name=\"{}\" Manufacturer=\"{}\""
        " DeviceUID=\"{}\" ModelUID=\"{}\""
        " CanBeDefault={} CanBeDefaultForSystemSounds={}"
        " SampleRate={} ChannelCount={}"
        " Latency={} SafetyOffset={} ZeroTimeStampPeriod={}",
        device_params.Name,
        device_params.Manufacturer,
        device_params.DeviceUID,
        device_params.ModelUID,
        device_params.CanBeDefault,
        device_params.CanBeDefaultForSystemSounds,
        device_params.SampleRate,
        device_params.ChannelCount,
        device_params.Latency,
        device_params.SafetyOffset,
        device_params.ZeroTimeStampPeriod);

    return device_params;
}

aspl::StreamParameters make_stream_params(const DeviceInfo& info)
{
    aspl::StreamParameters stream_params;

    const UInt32 num_chans = (UInt32)info.device_encoding.channel_count;

    stream_params.Direction = info.type == DeviceType::Sender ? aspl::Direction::Output
                                                              : aspl::Direction::Input;

    stream_params.Format.mSampleRate = UInt32(info.device_encoding.sample_rate);
    stream_params.Format.mFormatFlags = kAudioFormatFlagIsFloat |
                                        kAudioFormatFlagsNativeEndian |
                                        kAudioFormatFlagIsPacked;
    stream_params.Format.mBitsPerChannel = sizeof(Float32) * 8;
    stream_params.Format.mChannelsPerFrame = num_chans;
    stream_params.Format.mBytesPerFrame = sizeof(Float32) * num_chans;
    stream_params.Format.mFramesPerPacket = 1;
    stream_params.Format.mBytesPerPacket = sizeof(Float32) * num_chans;

    spdlog::debug(
        "stream parameters: Direction={} StartingChannel={}"
        " SampleRate={} FormatID={} FormatFlags={}"
        " BitsPerChannel={} ChannelsPerFrame={} BytesPerFrame={}"
        " FramesPerPacket={} BytesPerPacket={}"
        " Latency={}",
        (int)stream_params.Direction,
        stream_params.StartingChannel,
        stream_params.Format.mSampleRate,
        stream_params.Format.mFormatID,
        stream_params.Format.mFormatFlags,
        stream_params.Format.mBitsPerChannel,
        stream_params.Format.mChannelsPerFrame,
        stream_params.Format.mBytesPerFrame,
        stream_params.Format.mFramesPerPacket,
        stream_params.Format.mBytesPerPacket,
        stream_params.Latency);

    return stream_params;
}

} // namespace

Device::Device(std::shared_ptr<aspl::Plugin> hal_plugin,
    IndexAllocator& index_allocator,
    UidGenerator& uid_generator,
    const DeviceInfo& device_info)
    : index_allocator_(index_allocator)
    , uid_generator_(uid_generator)
    , hal_plugin_(hal_plugin)
{
    assert(hal_plugin_);

    // populate device info defaults
    info_ = device_info;

    if (info_.index == 0) {
        info_.index = index_allocator_.allocate_and_acquire();
    } else {
        index_allocator_.acquire(info_.index);
    }

    if (info_.uid.empty()) {
        info_.uid = uid_generator_.generate();
    }

    if (info_.name.empty()) {
        info_.name = fmt::format("Roc Virtual Device #{}", info_.index);
    }

    // channel_count will have been directly taken from rpc in the case of multi-track
    if (info_.device_encoding.channel_layout != ROC_CHANNEL_LAYOUT_MULTITRACK) {
        info_.device_encoding.channel_count =
            compute_channel_count(info_.device_encoding.channel_layout);
    }

    info_.device_encoding.buffer_samples = compute_buffer_samples(
        info_.device_encoding.buffer_length_ns, info_.device_encoding.sample_rate);

    spdlog::info(
        "creating device object,"
        " index={} uid={} type={} name=\"{}\" dev_rate={} dev_chans={} dev_buff={}",
        info_.index,
        info_.uid,
        info_.type,
        info_.name,
        info_.device_encoding.sample_rate,
        info_.device_encoding.channel_count,
        info_.device_encoding.buffer_samples);

    // create network sender or receiver
    if (info_.type == DeviceType::Sender) {
        net_transceiver_ = std::make_shared<Sender>(
            info_.uid, info_.device_encoding, *info_.sender_config);
    } else {
        net_transceiver_ = std::make_shared<Receiver>(
            info_.uid, info_.device_encoding, *info_.receiver_config);
    }
    if (!info_.enabled) {
        net_transceiver_->pause();
    }

    // create HAL device
    hal_device_ = std::make_shared<aspl::Device>(
        hal_plugin_->GetContext(), make_device_params(info_));
    // add single input our output stream with volume control
    hal_device_->AddStreamWithControlsAsync(make_stream_params(info_));

    // create HAL request handler
    req_handler_ = std::make_shared<RequestHandler>(
        info_.uid, info_.device_encoding, net_transceiver_);
    // register handler
    hal_device_->SetControlHandler(req_handler_);
    hal_device_->SetIOHandler(req_handler_);

    // bind or connect endpoints
    sort_endpoints_();

    for (auto& endpoint : info_.local_endpoints) {
        bind_endpoint_(endpoint);
    }
    for (auto& endpoint : info_.remote_endpoints) {
        connect_endpoint_(endpoint);
    }

    // enable or disable device
    toggle(info_.enabled);
}

Device::~Device()
{
    spdlog::info("destroying device object, index={} uid={} type={} name=\"{}\"",
        info_.index,
        info_.uid,
        info_.type,
        info_.name);

    toggle(false);

    if (info_.index != 0) {
        index_allocator_.release(info_.index);
    }
}

DeviceInfo Device::info()
{
    return info_;
}

void Device::toggle(bool enabled)
{
    info_.enabled = enabled;

    if (enabled) {
        if (!hal_plugin_->HasDevice(hal_device_)) {
            spdlog::debug("enabling device {}", info_.uid);

            hal_plugin_->AddDevice(hal_device_);
            net_transceiver_->resume();
        } else {
            spdlog::debug("device {} is already enabled", info_.uid);
        }
    } else {
        if (hal_plugin_->HasDevice(hal_device_)) {
            spdlog::debug("disabling device {}", info_.uid);

            hal_plugin_->RemoveDevice(hal_device_);
            net_transceiver_->pause();
        } else {
            spdlog::debug("device {} is already disabled", info_.uid);
        }
    }
}

DeviceEndpointInfo Device::bind(DeviceEndpointInfo endpoint_info)
{
    // may modify endpoint_info
    bind_endpoint_(endpoint_info);

    info_.local_endpoints.push_back(endpoint_info);
    sort_endpoints_();

    return endpoint_info;
}

DeviceEndpointInfo Device::connect(DeviceEndpointInfo endpoint_info)
{
    // may modify endpoint_info
    connect_endpoint_(endpoint_info);

    info_.remote_endpoints.push_back(endpoint_info);
    sort_endpoints_();

    return endpoint_info;
}

void Device::bind_endpoint_(DeviceEndpointInfo& endpoint_info)
{
    spdlog::info("binding device {} slot {} to endpoint {}",
        info_.uid,
        endpoint_info.slot,
        endpoint_info.uri);

    net_transceiver_->bind(endpoint_info);
}

void Device::connect_endpoint_(DeviceEndpointInfo& endpoint_info)
{
    spdlog::info("connecting device {} slot {} to endpoint {}",
        info_.uid,
        endpoint_info.slot,
        endpoint_info.uri);

    net_transceiver_->connect(endpoint_info);
}

void Device::sort_endpoints_()
{
    for (auto endpoints : {&info_.local_endpoints, &info_.remote_endpoints}) {
        std::sort(endpoints->begin(),
            endpoints->end(),
            [](const DeviceEndpointInfo& a, const DeviceEndpointInfo& b) {
                if (a.slot != b.slot) {
                    return a.slot < b.slot;
                } else {
                    return (int)a.interface < (int)b.interface;
                }
            });
    }
}

} // namespace rocvad
