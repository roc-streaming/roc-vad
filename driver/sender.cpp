/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "sender.hpp"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <cassert>
#include <stdexcept>

namespace rocvad {

Sender::Sender(const std::string& device_uid,
    const DeviceLocalEncoding& device_encoding,
    const DeviceSenderConfig& device_sender_config)
    : device_uid_(device_uid)
{
    int err = 0;

    roc_context_config net_context_config;
    memset(&net_context_config, 0, sizeof(net_context_config));

    if ((err = roc_context_open(&net_context_config, &net_context_)) < 0) {
        throw std::runtime_error(
            fmt::format("can't open network context: uid={} err={}", device_uid_, err));
    }

    roc_sender_config net_sender_config;
    memset(&net_sender_config, 0, sizeof(net_sender_config));

    net_sender_config.frame_encoding.rate = device_encoding.sample_rate;
    net_sender_config.frame_encoding.format = ROC_FORMAT_PCM_FLOAT32;
    net_sender_config.frame_encoding.channels = device_encoding.channel_layout;

    if (device_encoding.channel_layout == ROC_CHANNEL_LAYOUT_MULTITRACK) {
        net_sender_config.frame_encoding.tracks = device_encoding.channel_count;
    }

    if (device_sender_config.packet_encoding) {
        net_sender_config.packet_encoding = device_sender_config.packet_encoding->id;

        if ((err = roc_context_register_encoding(net_context_,
                 device_sender_config.packet_encoding->id,
                 &device_sender_config.packet_encoding->spec)) < 0) {
            throw std::invalid_argument(
                fmt::format("invalid packet encoding: uid={} err={}", device_uid_, err));
        }
    } else {
        net_sender_config.packet_encoding = ROC_PACKET_ENCODING_AVP_L16_STEREO;
    }

    net_sender_config.packet_length = device_sender_config.packet_length_ns;
    net_sender_config.packet_interleaving = device_sender_config.packet_interleaving;

    net_sender_config.fec_encoding = device_sender_config.fec_encoding;
    net_sender_config.fec_block_source_packets =
        device_sender_config.fec_block_source_packets;
    net_sender_config.fec_block_repair_packets =
        device_sender_config.fec_block_repair_packets;

    net_sender_config.clock_source = ROC_CLOCK_SOURCE_EXTERNAL;

    net_sender_config.latency_tuner_backend = device_sender_config.latency_tuner_backend;
    net_sender_config.latency_tuner_profile = device_sender_config.latency_tuner_profile;

    net_sender_config.resampler_backend = device_sender_config.resampler_backend;
    net_sender_config.resampler_profile = device_sender_config.resampler_profile;

    net_sender_config.target_latency = device_sender_config.target_latency_ns;
    net_sender_config.latency_tolerance = device_sender_config.latency_tolerance_ns;

    if ((err = roc_sender_open(net_context_, &net_sender_config, &net_sender_)) < 0) {
        throw std::invalid_argument(
            fmt::format("invalid sender config: uid={} err={}", device_uid_, err));
    }
}

Sender::~Sender()
{
    int err = 0;

    if (net_sender_) {
        if ((err = roc_sender_close(net_sender_)) < 0) {
            spdlog::warn(
                "can't properly close network sender: uid={} err={}", device_uid_, err);
        }
    }

    if (net_context_) {
        if ((err = roc_context_close(net_context_)) < 0) {
            spdlog::warn(
                "can't properly close network context: uid={} err={}", device_uid_, err);
        }
    }
}

void Sender::bind(DeviceEndpointInfo& endpoint_info)
{
    throw std::invalid_argument("sender device does not support bind() currently");
}

void Sender::connect(DeviceEndpointInfo& endpoint_info)
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

    if ((err = roc_sender_connect(
             net_sender_, endpoint_info.slot, endpoint_info.interface, endpoint)) < 0) {
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

void Sender::pause() noexcept
{
    // TODO: call roc_sender_pause()
    // https://github.com/roc-streaming/roc-toolkit/issues/678
}

void Sender::resume() noexcept
{
    // TODO: call roc_sender_resume()
    // https://github.com/roc-streaming/roc-toolkit/issues/678
}

void Sender::write(const float* samples, size_t n_samples) noexcept
{
    assert(samples);

    roc_frame frame;
    memset(&frame, 0, sizeof(frame));

    frame.samples = const_cast<float*>(samples);
    frame.samples_size = n_samples * sizeof(float);

    int err;
    if ((err = roc_sender_write(net_sender_, &frame)) < 0) {
        if (err_count_++ % err_report_freq_ == 0) {
            spdlog::warn(
                "can't write frame to network sender: uid={} err={} err_count={}",
                device_uid_,
                err,
                err_count_);
        }
    }
}

} // namespace rocvad
