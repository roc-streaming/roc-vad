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

    fmt::println("  type: {}", format_device_type(device_info.type()));
    fmt::println("  uid:  {}", device_info.uid());
    fmt::println("  name: {}", device_info.name());

    fmt::println("");

    fmt::println("  config:");

    if (device_info.has_device_encoding()) {
        fmt::println("");

        print_device_encoding(device_info.device_encoding());
    }

    if (device_info.has_sender_config()) {
        if (device_info.sender_config().has_packet_encoding()) {
            fmt::println("");

            print_packet_encoding(device_info.sender_config().packet_encoding());
        }

        fmt::println("");

        fmt::println("    packet_length:   {}",
            format_duration(device_info.sender_config().packet_length()));

        fmt::println("");

        fmt::println("    fec_encoding: {}",
            format_enum(fec_encoding_map, device_info.sender_config().fec_encoding()));
        fmt::println("    fec_nbsrc:    {}",
            device_info.sender_config().fec_block_source_packets());
        fmt::println("    fec_nbrpr:    {}",
            device_info.sender_config().fec_block_repair_packets());
    }

    if (device_info.has_receiver_config()) {
        fmt::println("");

        print_packet_encoding_list(device_info.receiver_config().packet_encodings());

        fmt::println("    target_latency: {}",
            format_duration(device_info.receiver_config().target_latency()));

        fmt::println("");

        fmt::println("    resampler_backend: {}",
            format_enum(resampler_backend_map,
                device_info.receiver_config().resampler_backend()));
        fmt::println("    resampler_profile: {}",
            format_enum(resampler_profile_map,
                device_info.receiver_config().resampler_profile()));
    }

    if (device_info.local_endpoints_size() != 0) {
        fmt::println("");

        fmt::println("  local endpoints:");

        print_endpoint_list(device_info.local_endpoints());
    }

    if (device_info.remote_endpoints_size() != 0) {
        fmt::println("");

        fmt::println("  remote endpoints:");

        print_endpoint_list(device_info.remote_endpoints());
    }
}

void print_device_list(const rvpb::RvDeviceList& device_list, bool show_info)
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
                format_device_type(device_info.type()),
                device_info.uid(),
                device_info.name());
        }
        is_first = false;
    }
}

void print_device_encoding(const rvpb::RvDeviceEncoding& encoding)
{
    fmt::println("device_encoding:");

    fmt::println("  rate:     {}", encoding.sample_rate());
    fmt::println("  channels: {}", //
        format_enum(channel_layout_map, encoding.channel_layout()));
}

void print_packet_encoding(const rvpb::RvPacketEncoding& encoding)
{
    fmt::println("packet_encoding:");

    fmt::println("  id:       {}", encoding.encoding_id());
    fmt::println("  rate:     {}", encoding.sample_rate());
    fmt::println("  format:   {}", //
        format_enum(sample_format_map, encoding.sample_format()));
    fmt::println("  channels: {}", //
        format_enum(channel_layout_map, encoding.channel_layout()));
}

void print_packet_encoding_list(
    const google::protobuf::RepeatedPtrField<rvpb::RvPacketEncoding>& encoding_list)
{
    for (const auto& encoding : encoding_list) {
        fmt::println("");

        print_packet_encoding(encoding);
    }
}

void print_endpoint_info(const rvpb::RvEndpointInfo& endpoint_info)
{
    fmt::println("endpoint:");

    fmt::println("  slot:      {}", endpoint_info.slot());
    fmt::println("  interface: {}", //
        format_enum(interface_map, endpoint_info.interface()));
    fmt::println("  uri:       {}", endpoint_info.uri());
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

    for (const auto& [slot, slot_endpoints] : endpoint_table) {
        fmt::println("");
        fmt::println("    slot {}:", slot);

        for (const auto& [interface, uri] : slot_endpoints) {
            fmt::println(
                "      {:<14} {}", format_enum(interface_map, interface) + ":", uri);
        }
    }
}

} // namespace rocvad
