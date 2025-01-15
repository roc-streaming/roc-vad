/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "index_allocator.hpp"

#include <roc/config.h>

#include <fmt/core.h>
#include <fmt/format.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace rocvad {

// Device type.
enum class DeviceType
{
    Sender,
    Receiver,
};

// Local encoding of virtual device.
struct DeviceLocalEncoding
{
    uint32_t sample_rate = 44100;
    roc_channel_layout channel_layout = ROC_CHANNEL_LAYOUT_STEREO;
    int64_t buffer_length_ns = 60'000'000;

    // exported via gRPC for multi-track and computed locally for mono and stereo
    size_t channel_count = 0;
    // not exported via gRPC, computed locally
    size_t buffer_samples = 0;
};

// Network encoding of packets.
// track_count only applies when format is multi-track, for mono and stereo it will be 0
struct DevicePacketEncoding
{
    roc_packet_encoding id = (roc_packet_encoding)0;
    roc_media_encoding spec = {
        .rate = 0,
        .format = (roc_format)0,
        .channels = (roc_channel_layout)0,
        .tracks = 0
    };
};

// Network parameters of sender device.
struct DeviceSenderConfig
{
    std::optional<DevicePacketEncoding> packet_encoding;
    uint64_t packet_length_ns = 0;
    bool packet_interleaving = false;

    roc_fec_encoding fec_encoding = ROC_FEC_ENCODING_RS8M;
    uint32_t fec_block_source_packets = 0;
    uint32_t fec_block_repair_packets = 0;

    roc_latency_tuner_backend latency_tuner_backend = ROC_LATENCY_TUNER_BACKEND_DEFAULT;
    roc_latency_tuner_profile latency_tuner_profile = ROC_LATENCY_TUNER_PROFILE_DEFAULT;

    roc_resampler_backend resampler_backend = ROC_RESAMPLER_BACKEND_DEFAULT;
    roc_resampler_profile resampler_profile = ROC_RESAMPLER_PROFILE_DEFAULT;

    int64_t target_latency_ns = 0;
    int64_t latency_tolerance_ns = 0;
};

// Network parameters of sender device.
struct DeviceReceiverConfig
{
    std::vector<DevicePacketEncoding> packet_encodings;

    roc_latency_tuner_backend latency_tuner_backend = ROC_LATENCY_TUNER_BACKEND_DEFAULT;
    roc_latency_tuner_profile latency_tuner_profile = ROC_LATENCY_TUNER_PROFILE_DEFAULT;

    roc_resampler_backend resampler_backend = ROC_RESAMPLER_BACKEND_DEFAULT;
    roc_resampler_profile resampler_profile = ROC_RESAMPLER_PROFILE_DEFAULT;

    int64_t target_latency_ns = 0;
    int64_t latency_tolerance_ns = 0;

    int64_t no_playback_timeout_ns = 0;
    int64_t choppy_playback_timeout_ns = 0;
};

// Device endpoint info.
struct DeviceEndpointInfo
{
    uint32_t slot = ROC_SLOT_DEFAULT;

    roc_interface interface = (roc_interface)-1;
    std::string uri = {};
};

// Device info.
struct DeviceInfo
{
    DeviceType type = DeviceType::Sender;

    IndexAllocator::index_t index = 0;
    std::string uid;
    std::string name;
    bool enabled = true;

    DeviceLocalEncoding device_encoding;

    std::optional<DeviceSenderConfig> sender_config;
    std::optional<DeviceReceiverConfig> receiver_config;

    std::vector<DeviceEndpointInfo> local_endpoints;
    std::vector<DeviceEndpointInfo> remote_endpoints;
};

} // namespace rocvad

//! Device type formatter.
template <>
struct fmt::formatter<rocvad::DeviceType> : fmt::formatter<std::string_view>
{
    auto format(rocvad::DeviceType device_type, fmt::format_context& ctx) const
    {
        return fmt::formatter<std::string_view>::format(
            device_type == rocvad::DeviceType::Sender ? "sender" : "receiver", ctx);
    }
};
