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
#include "format.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <map>
#include <tuple>
#include <vector>

namespace rocvad {

void print_driver_and_client_info(const rvpb::RvDriverInfo& driver_info)
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

void print_device_info(const rvpb::RvDeviceInfo& device_info)
{
    fmt::println("device #{}", device_info.index());

    fmt::println("");

    fmt::println("  type:   {}", format_device_type(device_info.type()));
    fmt::println("  uid:    {}", device_info.uid());
    fmt::println("  name:   {}", device_info.name());
    fmt::println("  state:  {}", device_info.enabled() ? "on" : "off");

    if (device_info.has_device_encoding()) {
        fmt::println("");

        print_device_encoding(device_info.device_encoding());
    }

    if (device_info.has_sender_config()) {
        fmt::println("");
        fmt::println("  sender_config:");

        fmt::println("    packet_length:        {}",
            format_duration(device_info.sender_config().packet_length()));
        fmt::println("    packet_interleaving:  {}",
            device_info.sender_config().packet_interleaving());

        if (device_info.sender_config().has_packet_encoding()) {
            print_packet_encoding(device_info.sender_config().packet_encoding());
        } else {
            fmt::println("    packet_encoding:      default");
        }

        fmt::println("");
        fmt::println("    fec_encoding:     {}",
            format_enum(fec_encoding_map, device_info.sender_config().fec_encoding()));
        fmt::println("    fec_block_nbsrc:  {}",
            format_size(device_info.sender_config().fec_block_source_packets()));
        fmt::println("    fec_block_nbrpr:  {}",
            format_size(device_info.sender_config().fec_block_repair_packets()));

        fmt::println("");
        fmt::println("    resampler_backend:  {}",
            format_enum(
                resampler_backend_map, device_info.sender_config().resampler_backend()));
        fmt::println("    resampler_profile:  {}",
            format_enum(
                resampler_profile_map, device_info.sender_config().resampler_profile()));

        fmt::println("");
        fmt::println("    latency_backend:  {}",
            format_enum(latency_tuner_backend_map,
                device_info.sender_config().latency_tuner_backend()));
        fmt::println("    latency_profile:  {}",
            format_enum(latency_tuner_profile_map,
                device_info.sender_config().latency_tuner_profile()));

        fmt::println("");
        fmt::println("    target_latency:     {}",
            format_duration(device_info.sender_config().target_latency()));
        fmt::println("    latency_tolerance:  {}",
            format_duration(device_info.sender_config().latency_tolerance()));
    }

    if (device_info.has_receiver_config()) {
        fmt::println("");
        fmt::println("  receiver_config:");

        for (const auto& encoding : device_info.receiver_config().packet_encodings()) {
            print_packet_encoding(encoding);
            fmt::println("");
        }

        fmt::println("    resampler_backend:  {}",
            format_enum(resampler_backend_map,
                device_info.receiver_config().resampler_backend()));
        fmt::println("    resampler_profile:  {}",
            format_enum(resampler_profile_map,
                device_info.receiver_config().resampler_profile()));

        fmt::println("");
        fmt::println("    latency_backend:  {}",
            format_enum(latency_tuner_backend_map,
                device_info.receiver_config().latency_tuner_backend()));
        fmt::println("    latency_profile:  {}",
            format_enum(latency_tuner_profile_map,
                device_info.receiver_config().latency_tuner_profile()));

        fmt::println("");
        fmt::println("    target_latency:    {}",
            format_duration(device_info.receiver_config().target_latency()));
        fmt::println("    latency_tolerance:  {}",
            format_duration(device_info.receiver_config().latency_tolerance()));

        fmt::println("");
        fmt::println("    no_play_timeout:      {}",
            format_duration(device_info.receiver_config().no_playback_timeout()));
        fmt::println("    choppy_play_timeout:  {}",
            format_duration(device_info.receiver_config().choppy_playback_timeout()));
    }

    if (device_info.local_endpoints_size() != 0) {
        fmt::println("");
        fmt::println("  local_endpoints:");

        print_endpoint_list(device_info.local_endpoints());
    }

    if (device_info.remote_endpoints_size() != 0) {
        fmt::println("");
        fmt::println("  remote_endpoints:");

        print_endpoint_list(device_info.remote_endpoints());
    }
}

void print_device_list(const rvpb::RvDeviceList& device_list, bool show_info)
{
    if (!show_info) {
        fmt::println("{:<8} {:<10} {:<7} {:<30} {}", //
            "index",
            "type",
            "state",
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
            fmt::println("{:<8} {:<10} {:<7} {:<30} {}",
                device_info.index(),
                format_device_type(device_info.type()),
                device_info.enabled() ? "on" : "off",
                device_info.uid(),
                device_info.name());
        }
        is_first = false;
    }
}

void print_device_encoding(const rvpb::RvDeviceEncoding& encoding)
{
    fmt::println("  device_encoding:");
    fmt::println("    rate:      {}Hz", encoding.sample_rate());
    fmt::println("    channels:  {}", //
        format_enum(channel_layout_map, encoding.channel_layout()));
    if (encoding.channel_layout() == rvpb::RV_CHANNEL_LAYOUT_MULTITRACK && encoding.has_track_count()) {
        fmt::println("    tracks:  {}", encoding.track_count());
    }
    fmt::println("    buffer:    {}", //
        format_duration(encoding.buffer_length()));
}

void print_packet_encoding(const rvpb::RvPacketEncoding& encoding)
{
    fmt::println("    packet_encoding:");
    fmt::println("      id:        {}", encoding.encoding_id());
    fmt::println("      rate:      {}Hz", encoding.sample_rate());
    fmt::println("      format:    {}", //
        format_enum(sample_format_map, encoding.sample_format()));
    fmt::println("      channels:  {}", //
        format_enum(channel_layout_map, encoding.channel_layout()));
    if (encoding.channel_layout() == rvpb::RV_CHANNEL_LAYOUT_MULTITRACK && encoding.has_track_count()) {
        fmt::println("      tracks:  {}", encoding.track_count());
    }
}

void print_endpoint_info(const rvpb::RvEndpointInfo& endpoint_info)
{
    fmt::println("endpoint:");
    fmt::println("  slot:       {}", endpoint_info.slot());
    fmt::println("  interface:  {}", //
        format_enum(interface_map, endpoint_info.interface()));
    fmt::println("  uri:        {}", endpoint_info.uri());
}

void print_endpoint_list(
    const google::protobuf::RepeatedPtrField<rvpb::RvEndpointInfo>& endpoint_list)
{
    std::map<unsigned int, std::vector<std::tuple<rvpb::RvInterface, std::string>>>
        endpoint_table;

    for (const auto& endpoint_info : endpoint_list) {
        endpoint_table[endpoint_info.slot()].push_back(
            std::make_tuple(endpoint_info.interface(), endpoint_info.uri()));
    }

    for (auto& [_, slot_endpoints] : endpoint_table) {
        std::sort(slot_endpoints.begin(), slot_endpoints.end(), [](auto a, auto b) {
            // Sort endpoints in slot by interface.
            return (int)std::get<0>(a) < (int)std::get<0>(b);
        });
    }

    bool is_first = true;

    for (const auto& [slot, slot_endpoints] : endpoint_table) {
        if (!is_first) {
            fmt::println("");
        }
        fmt::println("    slot {}:", slot);

        for (const auto& [interface, uri] : slot_endpoints) {
            fmt::println(
                "      {:<14} {}", format_enum(interface_map, interface) + ":", uri);
        }
        is_first = false;
    }
}

} // namespace rocvad
