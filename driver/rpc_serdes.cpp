/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "rpc_serdes.hpp"
#include "enum_map.hpp"

#include <fmt/core.h>
#include <google/protobuf/util/time_util.h>

#include <stdexcept>

namespace rocvad {

namespace {

template <class Map, class Enum>
auto enum_from_rpc(const char* name, const Map& map, Enum in)
{
    for (auto entry : map) {
        if (std::get<0>(entry) == in) {
            return std::get<1>(entry);
        }
    }
    throw std::invalid_argument(fmt::format("invalid {} value {}", name, (int)in));
}

template <class Map, class Enum>
auto enum_to_rpc(const char* name, const Map& map, Enum in)
{
    for (auto entry : map) {
        if (std::get<1>(entry) == in) {
            return std::get<0>(entry);
        }
    }
    throw std::runtime_error(fmt::format("invalid {} value {}", name, (int)in));
}

uint64_t nanoseconds_from_rpc(const char* name, google::protobuf::Duration duration)
{
    const int64_t nanosecond =
        google::protobuf::util::TimeUtil::DurationToNanoseconds(duration);

    if (nanosecond < 0) {
        throw std::invalid_argument(fmt::format("invalid negative {} value {}",
            name,
            google::protobuf::util::TimeUtil::ToString(duration)));
    }

    return (uint64_t)nanosecond;
}

google::protobuf::Duration nanoseconds_to_rpc(const char* name, uint64_t nanoseconds)
{
    return google::protobuf::util::TimeUtil::NanosecondsToDuration((int64_t)nanoseconds);
}

} // namespace

void device_list_from_rpc(std::vector<DeviceInfo>& out, const PrDeviceList& in)
{
    out.clear();

    for (const auto& in_info : in.devices()) {
        DeviceInfo out_info;
        device_info_from_rpc(out_info, in_info);

        out.push_back(out_info);
    }
}

void device_list_to_rpc(PrDeviceList& out, const std::vector<DeviceInfo>& in)
{
    for (const auto& in_info : in) {
        device_info_to_rpc(*out.add_devices(), in_info);
    }
}

void device_info_from_rpc(DeviceInfo& out, const PrDeviceInfo& in)
{
    // type
    switch (in.type()) {
    case PR_DEVICE_TYPE_SENDER:
        out.type = DeviceType::Sender;
        out.sender_config = DeviceSenderConfig {};

        if (in.has_receiver_config()) {
            throw std::invalid_argument(
                "receiver config should not be present for sender device");
        }

        break;

    case PR_DEVICE_TYPE_RECEIVER:
        out.type = DeviceType::Receiver;
        out.receiver_config = DeviceReceiverConfig {};

        if (in.has_sender_config()) {
            throw std::invalid_argument(
                "sender config should not be present for sender device");
        }

        break;

    default:
        throw std::invalid_argument(
            fmt::format("invalid device type {}", (int)in.type()));
    }

    // identification
    if (in.has_index()) {
        if (in.index() == 0) {
            throw std::invalid_argument(
                fmt::format("device index should be either unset or non-zero"));
        }
        out.index = in.index();
    }

    if (in.has_uid()) {
        if (in.uid().empty()) {
            throw std::invalid_argument(
                fmt::format("device uid should be either unset or non-empty"));
        }
        out.uid = in.uid();
    }

    if (in.has_name()) {
        if (in.name().empty()) {
            throw std::invalid_argument(
                fmt::format("device name should be either unset or non-empty"));
        }
        out.name = in.name();
    }

    // local config
    if (in.local_config().has_sample_rate()) {
        out.local_config.sample_rate = in.local_config().sample_rate();

        if (out.local_config.sample_rate == 0) {
            throw std::invalid_argument("sample_rate should not be zero");
        }
    }

    if (in.local_config().has_channel_layout()) {
        out.local_config.channel_layout = enum_from_rpc(
            "channel_layout", channel_layout_map, in.local_config().channel_layout());
    }

    // sender config
    if (in.has_sender_config()) {
        if (in.sender_config().has_packet_encoding()) {
            out.sender_config->packet_encoding = enum_from_rpc("packet_encoding",
                packet_encoding_map,
                in.sender_config().packet_encoding());
        }

        if (in.sender_config().has_packet_length()) {
            out.sender_config->packet_length_ns =
                nanoseconds_from_rpc("packet_length", in.sender_config().packet_length());

            if (out.sender_config->packet_length_ns == 0) {
                throw std::invalid_argument("packet_length should not be zero");
            }
        }

        if (in.sender_config().has_fec_encoding()) {
            out.sender_config->fec_encoding = enum_from_rpc(
                "fec_encoding", fec_encoding_map, in.sender_config().fec_encoding());
        }

        if (in.sender_config().has_fec_block_source_packets()) {
            out.sender_config->fec_block_source_packets =
                in.sender_config().fec_block_source_packets();

            if (out.sender_config->fec_block_source_packets == 0 &&
                out.sender_config->fec_encoding != ROC_FEC_ENCODING_DISABLE) {
                throw std::invalid_argument(
                    "fec_block_source_packets should not be zero if "
                    "fec_encoding is not set to PR_FEC_ENCODING_DISABLE");
            }
        }

        if (in.sender_config().has_fec_block_repair_packets()) {
            out.sender_config->fec_block_repair_packets =
                in.sender_config().fec_block_repair_packets();

            if (out.sender_config->fec_block_repair_packets == 0 &&
                out.sender_config->fec_encoding != ROC_FEC_ENCODING_DISABLE) {
                throw std::invalid_argument(
                    "fec_block_repair_packets should not be zero if "
                    "fec_encoding is not set to PR_FEC_ENCODING_DISABLE");
            }
        }
    }

    // receiver config
    if (in.has_receiver_config()) {
        if (in.receiver_config().has_target_latency()) {
            out.receiver_config->target_latency_ns = nanoseconds_from_rpc(
                "target_latency", in.receiver_config().target_latency());

            if (out.receiver_config->target_latency_ns == 0) {
                throw std::invalid_argument("target_latency should not be zero");
            }
        }

        if (in.receiver_config().has_resampler_backend()) {
            out.receiver_config->resampler_backend = enum_from_rpc("resampler_backend",
                resampler_backend_map,
                in.receiver_config().resampler_backend());
        }

        if (in.receiver_config().has_resampler_profile()) {
            out.receiver_config->resampler_profile = enum_from_rpc("resampler_profile",
                resampler_profile_map,
                in.receiver_config().resampler_profile());
        }
    }

    // endpoints
    for (const auto& in_info : in.local_endpoints()) {
        DeviceEndpointInfo out_info;
        endpoint_info_from_rpc(out_info, in_info);

        out.local_endpoints.push_back(out_info);
    }

    for (const auto& in_info : in.remote_endpoints()) {
        DeviceEndpointInfo out_info;
        endpoint_info_from_rpc(out_info, in_info);

        out.remote_endpoints.push_back(out_info);
    }
}

void device_info_to_rpc(PrDeviceInfo& out, const DeviceInfo& in)
{
    // type
    out.set_type(
        in.type == DeviceType::Sender ? PR_DEVICE_TYPE_SENDER : PR_DEVICE_TYPE_RECEIVER);

    // identification
    out.set_index(in.index);
    out.set_uid(in.uid);
    out.set_name(in.name);

    // local config
    out.mutable_local_config()->set_sample_rate(in.local_config.sample_rate);
    out.mutable_local_config()->set_channel_layout(enum_to_rpc(
        "channel_layout", channel_layout_map, in.local_config.channel_layout));

    // sender config
    if (in.sender_config) {
        out.mutable_sender_config()->set_packet_encoding(enum_to_rpc(
            "packet_encoding", packet_encoding_map, in.sender_config->packet_encoding));

        *out.mutable_sender_config()->mutable_packet_length() =
            nanoseconds_to_rpc("packet_length", in.sender_config->packet_length_ns);

        out.mutable_sender_config()->set_fec_encoding(enum_to_rpc(
            "fec_encoding", fec_encoding_map, in.sender_config->fec_encoding));

        out.mutable_sender_config()->set_fec_block_source_packets(
            in.sender_config->fec_block_source_packets);

        out.mutable_sender_config()->set_fec_block_repair_packets(
            in.sender_config->fec_block_repair_packets);
    }

    // receiver config
    if (in.receiver_config) {
        *out.mutable_receiver_config()->mutable_target_latency() =
            nanoseconds_to_rpc("target_latency", in.receiver_config->target_latency_ns);

        out.mutable_receiver_config()->set_resampler_backend(
            enum_to_rpc("resampler_backend",
                resampler_backend_map,
                in.receiver_config->resampler_backend));

        out.mutable_receiver_config()->set_resampler_profile(
            enum_to_rpc("resampler_profile",
                resampler_profile_map,
                in.receiver_config->resampler_profile));
    }

    // endpoints
    for (const auto& in_info : in.local_endpoints) {
        PrEndpointInfo out_info;
        endpoint_info_to_rpc(out_info, in_info);

        *out.add_local_endpoints() = out_info;
    }

    for (const auto& in_info : in.remote_endpoints) {
        PrEndpointInfo out_info;
        endpoint_info_to_rpc(out_info, in_info);

        *out.add_remote_endpoints() = out_info;
    }
}

void endpoint_info_from_rpc(DeviceEndpointInfo& out, const PrEndpointInfo& in)
{
    if (in.has_slot()) {
        out.slot = in.slot();
    }

    out.interface = enum_from_rpc("endpoint interface", interface_map, in.interface());
    out.uri = in.uri();

    if (out.uri.empty()) {
        throw std::invalid_argument("endpoint uri should not be empty");
    }
}

void endpoint_info_to_rpc(PrEndpointInfo& out, const DeviceEndpointInfo& in)
{
    out.set_slot(in.slot);
    out.set_interface(enum_to_rpc("endpoint interface", interface_map, in.interface));
    out.set_uri(in.uri);
}

} // namespace rocvad
