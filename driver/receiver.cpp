/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "receiver.hpp"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <cassert>
#include <stdexcept>

namespace rocvad {

Receiver::Receiver(const std::string& device_uid,
    const DeviceLocalEncoding& device_encoding,
    const DeviceReceiverConfig& device_receiver_config)
    : device_uid_(device_uid)
{
    int err = 0;

    roc_context_config net_context_config;
    memset(&net_context_config, 0, sizeof(net_context_config));

    if ((err = roc_context_open(&net_context_config, &net_context_)) < 0) {
        throw std::runtime_error(
            fmt::format("can't open network context: uid={} err={}", device_uid_, err));
    }

    for (const auto& packet_encoding : device_receiver_config.packet_encodings) {
        if ((err = roc_context_register_encoding(
                 net_context_, packet_encoding.id, &packet_encoding.spec)) < 0) {
            throw std::invalid_argument(
                fmt::format("invalid packet encoding: uid={} err={}", device_uid_, err));
        }
    }

    roc_receiver_config net_receiver_config;
    memset(&net_receiver_config, 0, sizeof(net_receiver_config));

    net_receiver_config.frame_encoding.rate = device_encoding.sample_rate;
    net_receiver_config.frame_encoding.format = ROC_FORMAT_PCM_FLOAT32;
    net_receiver_config.frame_encoding.channels = device_encoding.channel_layout;
    if (device_encoding.channel_layout == ROC_CHANNEL_LAYOUT_MULTITRACK) {
        net_receiver_config.frame_encoding.tracks = device_encoding.channel_count;
    }

    net_receiver_config.clock_source = ROC_CLOCK_SOURCE_EXTERNAL;

    net_receiver_config.latency_tuner_backend =
        device_receiver_config.latency_tuner_backend;
    net_receiver_config.latency_tuner_profile =
        device_receiver_config.latency_tuner_profile;

    net_receiver_config.resampler_backend = device_receiver_config.resampler_backend;
    net_receiver_config.resampler_profile = device_receiver_config.resampler_profile;

    net_receiver_config.target_latency = device_receiver_config.target_latency_ns;
    net_receiver_config.latency_tolerance = device_receiver_config.latency_tolerance_ns;

    net_receiver_config.no_playback_timeout =
        device_receiver_config.no_playback_timeout_ns;
    net_receiver_config.choppy_playback_timeout =
        device_receiver_config.choppy_playback_timeout_ns;

    if ((err = roc_receiver_open(net_context_, &net_receiver_config, &net_receiver_)) <
        0) {
        throw std::invalid_argument(
            fmt::format("invalid receiver config: uid={} err={}", device_uid_, err));
    }
}

Receiver::~Receiver()
{
    int err = 0;

    if (net_receiver_) {
        if ((err = roc_receiver_close(net_receiver_)) < 0) {
            spdlog::warn(
                "can't properly close network receiver: uid={} err={}", device_uid_, err);
        }
    }

    if (net_context_) {
        if ((err = roc_context_close(net_context_)) < 0) {
            spdlog::warn(
                "can't properly close network context: uid={} err={}", device_uid_, err);
        }
    }
}

void Receiver::bind(DeviceEndpointInfo& endpoint_info)
{
    int err = 0;

    roc_endpoint* endpoint = nullptr;

    if ((err = roc_endpoint_allocate(&endpoint)) < 0) {
        throw std::runtime_error(fmt::format(
            "can't allocate network endpoint: uid={} err={}", device_uid_, err));
    }

    if ((err = roc_endpoint_set_uri(endpoint, endpoint_info.uri.c_str())) < 0) {
        throw std::invalid_argument(fmt::format("invalid endpoint: uid={} uri={} err={}",
            device_uid_,
            endpoint_info.uri,
            err));
    }

    if ((err = roc_receiver_bind(
             net_receiver_, endpoint_info.slot, endpoint_info.interface, endpoint)) < 0) {
        throw std::invalid_argument(fmt::format("invalid endpoint: uid={} uri={} err={}",
            device_uid_,
            endpoint_info.uri,
            err));
    }

    if ((err = roc_endpoint_deallocate(endpoint)) < 0) {
        spdlog::warn("can't properly deallocate network endpoint: uid={} err={}",
            device_uid_,
            err);
    }
}

void Receiver::connect(DeviceEndpointInfo& endpoint_info)
{
    throw std::invalid_argument("receiver device does not support connect() currently");
}

void Receiver::pause() noexcept
{
    // TODO: call roc_receiver_pause()
    // https://github.com/roc-streaming/roc-toolkit/issues/678
}

void Receiver::resume() noexcept
{
    // TODO: call roc_receiver_resume()
    // https://github.com/roc-streaming/roc-toolkit/issues/678
}

void Receiver::read(float* samples, size_t n_samples) noexcept
{
    assert(samples);

    roc_frame frame;
    memset(&frame, 0, sizeof(frame));

    frame.samples = samples;
    frame.samples_size = n_samples * sizeof(float);

    int err;
    if ((err = roc_receiver_read(net_receiver_, &frame)) < 0) {
        if (err_count_++ % err_report_freq_ == 0) {
            spdlog::warn(
                "can't read frame from network receiver: uid={} err={} err_count={}",
                device_uid_,
                err,
                err_count_);
        }
    }
}

} // namespace rocvad
