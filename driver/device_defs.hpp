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

// Local parameters of device.
struct DeviceLocalConfig
{
    uint32_t sample_rate = 44100;
    roc_channel_set channel_set = ROC_CHANNEL_SET_STEREO;
};

// Network parameters of sender device.
struct DeviceSenderConfig
{
    roc_packet_encoding packet_encoding = ROC_PACKET_ENCODING_AVP_L16;
    uint64_t packet_length_ns = 7'000'000; // 7ms

    roc_fec_encoding fec_encoding = ROC_FEC_ENCODING_RS8M;
    uint32_t fec_block_source_packets = 20;
    uint32_t fec_block_repair_packets = 10;
};

// Network parameters of sender device.
struct DeviceReceiverConfig
{
    uint64_t target_latency_ns = 100'000'000; // 100ms

    roc_resampler_backend resampler_backend = ROC_RESAMPLER_BACKEND_SPEEX;
    roc_resampler_profile resampler_profile = ROC_RESAMPLER_PROFILE_MEDIUM;
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

    DeviceLocalConfig local_config;

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
