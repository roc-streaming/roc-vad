/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "device.hpp"
#include "build_info.hpp"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace rocvad {

namespace {

aspl::DeviceParameters make_device_params(const DeviceInfo& info)
{
    aspl::DeviceParameters device_params;

    device_params.Name = info.name;
    device_params.Manufacturer = BuildInfo::driver_manufacturer;
    device_params.DeviceUID = info.uid;
    device_params.ModelUID = BuildInfo::driver_bundle_id;

    device_params.SampleRate = info.device_encoding.sample_rate;

    switch (info.device_encoding.channel_layout) {
    case ROC_CHANNEL_LAYOUT_MONO:
        device_params.ChannelCount = 1;
        break;

    case ROC_CHANNEL_LAYOUT_STEREO:
        device_params.ChannelCount = 2;
        break;

    default:
        throw std::runtime_error("selected channel_layout not supported by device");
    }

    device_params.EnableMixing = true;
    device_params.EnableRealtimeTracing = false;

    return device_params;
}

} // namespace

Device::Device(std::shared_ptr<aspl::Plugin> plugin,
    IndexAllocator& index_allocator,
    UidGenerator& uid_generator,
    const DeviceInfo& device_info)
    : index_allocator_(index_allocator)
    , uid_generator_(uid_generator)
    , plugin_(plugin)
    , info_(device_info)
{
    assert(plugin_);

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

    spdlog::info("creating device object, index={} uid={} type={} name=\"{}\"",
        info_.index,
        info_.uid,
        info_.type,
        info_.name);

    device_ =
        std::make_shared<aspl::Device>(plugin_->GetContext(), make_device_params(info_));

    device_->AddStreamWithControlsAsync(info_.type == DeviceType::Sender
                                            ? aspl::Direction::Output
                                            : aspl::Direction::Input);

    plugin_->AddDevice(device_);

    // TODO: roc_open

    sort_endpoints_();

    for (auto& endpoint : info_.local_endpoints) {
        bind_endpoint_(endpoint);
    }

    for (auto& endpoint : info_.remote_endpoints) {
        connect_endpoint_(endpoint);
    }
}

Device::~Device()
{
    spdlog::info("destroying device object, index={} uid={} type={} name=\"{}\"",
        info_.index,
        info_.uid,
        info_.type,
        info_.name);

    // TODO: roc_close

    if (device_) {
        plugin_->RemoveDevice(device_);
    }

    if (info_.index != 0) {
        index_allocator_.release(info_.index);
    }
}

DeviceInfo Device::info()
{
    return info_;
}

DeviceEndpointInfo Device::bind(DeviceEndpointInfo endpoint_info)
{
    bind_endpoint_(endpoint_info);

    info_.local_endpoints.push_back(endpoint_info);
    sort_endpoints_();

    return endpoint_info;
}

DeviceEndpointInfo Device::connect(DeviceEndpointInfo endpoint_info)
{
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

    // TODO: roc_bind
}

void Device::connect_endpoint_(DeviceEndpointInfo& endpoint_info)
{
    spdlog::info("connecting device {} slot {} to endpoint {}",
        info_.uid,
        endpoint_info.slot,
        endpoint_info.uri);

    // TODO: roc_connect
}

void Device::sort_endpoints_()
{
    std::sort(info_.local_endpoints.begin(),
        info_.local_endpoints.end(),
        [](const DeviceEndpointInfo& a, const DeviceEndpointInfo& b) {
            if (a.slot != b.slot) {
                return a.slot < b.slot;
            } else {
                return (int)a.interface < (int)b.interface;
            }
        });
}

} // namespace rocvad
