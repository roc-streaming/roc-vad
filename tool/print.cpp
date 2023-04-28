/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "print.hpp"
#include "build_info.hpp"
#include "driver_protocol.hpp"
#include "enum_map.hpp"

#include <fmt/core.h>
#include <google/protobuf/util/time_util.h>

#include <array>
#include <cstdint>
#include <tuple>

namespace rocvad {

namespace {

std::string format_duration(uint64_t nanoseconds)
{
    uint64_t multiplier = 0;
    const char* suffix = nullptr;

    for (auto [m, s] : std::array<std::tuple<uint64_t, const char*>, 6> {{
             {1, "ns"},
             {1'000, "us"},
             {1'000'000, "ms"},
             {1'000'000'000, "s"},
             {60'000'000'000, "m"},
             {3660'000'000'000, "h"},
         }}) {
        if (nanoseconds >= m) {
            multiplier = m;
            suffix = s;
        }
    }

    return fmt::format("{}{}", (double)nanoseconds / multiplier, suffix);
}

std::string format_duration(google::protobuf::Duration duration)
{
    const int64_t nanoseconds =
        google::protobuf::util::TimeUtil::DurationToNanoseconds(duration);

    return format_duration((uint64_t)nanoseconds);
}

template <class Map, class Enum>
std::string format_enum(const Map& map, Enum in)
{
    for (auto entry : map) {
        if (std::get<0>(entry) == in) {
            return std::get<2>(entry);
        }
    }

    return fmt::format("unknown ({})", (int)in);
}

} // namespace

void print_driver_and_client_info(const PrDriverInfo& driver_info)
{
    fmt::println("driver is loaded");

    fmt::println("");

    fmt::println("driver:");
    fmt::println("  version: {}", driver_info.version());
    fmt::println("  commit:  {}", driver_info.commit().substr(0, 7));

    fmt::println("");

    fmt::println("client:");
    fmt::println("  version: {}", BuildInfo::git_version);
    fmt::println("  commit:  {}", std::string(BuildInfo::git_commit).substr(0, 7));
}

void print_device_info(const PrDeviceInfo& device_info)
{
    fmt::println("device #{}", device_info.index());

    fmt::println("");

    fmt::println("  type: {}",
        device_info.type() == PR_DEVICE_TYPE_SENDER ? "sender" : "receiver");
    fmt::println("  uid:  {}", device_info.uid());
    fmt::println("  name: {}", device_info.name());

    if (device_info.has_local_config()) {
        fmt::println("");

        fmt::println("  sample_rate: {}", device_info.local_config().sample_rate());
        fmt::println("  channel_set: {}",
            format_enum(channel_set_map, device_info.local_config().channel_set()));
    }

    if (device_info.has_sender_config()) {
        fmt::println("");

        fmt::println("  packet_encoding: {}",
            format_enum(
                packet_encoding_map, device_info.sender_config().packet_encoding()));
        fmt::println("  packet_length:   {}",
            format_duration(device_info.sender_config().packet_length()));

        fmt::println("");

        fmt::println("  fec_encoding: {}",
            format_enum(fec_encoding_map, device_info.sender_config().fec_encoding()));
        fmt::println(
            "  fec_nbsrc:    {}", device_info.sender_config().fec_block_source_packets());
        fmt::println(
            "  fec_nbrpr:    {}", device_info.sender_config().fec_block_repair_packets());
    }

    if (device_info.has_receiver_config()) {
        fmt::println("");

        fmt::println("  target_latency: {}",
            format_duration(device_info.receiver_config().target_latency()));

        fmt::println("");

        fmt::println("  resampler_backend: {}",
            format_enum(resampler_backend_map,
                device_info.receiver_config().resampler_backend()));
        fmt::println("  resampler_profile: {}",
            format_enum(resampler_profile_map,
                device_info.receiver_config().resampler_profile()));
    }
}

void print_device_list(const PrDeviceList& device_list, bool show_info)
{
    if (!show_info) {
        fmt::println("{:<8} {:<10} {:<22} {}", //
            "index",
            "type",
            "uid",
            "name");
    }

    bool is_first = true;

    for (const auto& device_info : device_list.devices()) {
        if (show_info) {
            if (!is_first) {
                fmt::println("");
            }
            print_device_info(device_info);
        } else {
            fmt::println("{:<8} {:<10} {:<22} {}",
                device_info.index(),
                device_info.type() == PR_DEVICE_TYPE_SENDER ? "sender" : "receiver",
                device_info.uid(),
                device_info.name());
        }
        is_first = false;
    }
}

} // namespace rocvad
