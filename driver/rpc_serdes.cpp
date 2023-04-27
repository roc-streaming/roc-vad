/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "rpc_serdes.hpp"

#include <fmt/core.h>
#include <google/protobuf/util/time_util.h>

#include <array>
#include <optional>
#include <stdexcept>
#include <utility>

namespace rocvad {

namespace {

std::array<std::optional<std::pair<PrChannelSet, roc_channel_set>>,
    PrChannelSet_ARRAYSIZE>
    channel_set_map {{
        {{PR_CHANNEL_SET_STEREO, ROC_CHANNEL_SET_STEREO}},
    }};

std::array<std::optional<std::pair<PrPacketEncoding, roc_packet_encoding>>,
    PrPacketEncoding_ARRAYSIZE>
    packet_encoding_map {{
        {{PR_PACKET_ENCODING_AVP_L16, ROC_PACKET_ENCODING_AVP_L16}},
    }};

std::array<std::optional<std::pair<PrFecEncoding, roc_fec_encoding>>,
    PrFecEncoding_ARRAYSIZE>
    fec_encoding_map {{
        {{PR_FEC_ENCODING_DISABLE, ROC_FEC_ENCODING_DISABLE}},
        {{PR_FEC_ENCODING_RS8M, ROC_FEC_ENCODING_RS8M}},
        {{PR_FEC_ENCODING_LDPC_STAIRCASE, ROC_FEC_ENCODING_LDPC_STAIRCASE}},
    }};

std::array<std::optional<std::pair<PrResamplerBackend, roc_resampler_backend>>,
    PrResamplerBackend_ARRAYSIZE>
    resampler_backend_map {{
        {{PR_RESAMPLER_BACKEND_BUILTIN, ROC_RESAMPLER_BACKEND_BUILTIN}},
        {{PR_RESAMPLER_BACKEND_SPEEX, ROC_RESAMPLER_BACKEND_SPEEX}},
    }};

std::array<std::optional<std::pair<PrResamplerProfile, roc_resampler_profile>>,
    PrResamplerProfile_ARRAYSIZE>
    resampler_profile_map {{
        {{PR_RESAMPLER_PROFILE_DISABLE, ROC_RESAMPLER_PROFILE_DISABLE}},
        {{PR_RESAMPLER_PROFILE_HIGH, ROC_RESAMPLER_PROFILE_HIGH}},
        {{PR_RESAMPLER_PROFILE_MEDIUM, ROC_RESAMPLER_PROFILE_MEDIUM}},
        {{PR_RESAMPLER_PROFILE_LOW, ROC_RESAMPLER_PROFILE_LOW}},
    }};

template <class Map, class Enum>
auto enum_from_rpc(const char* name, const Map& map, Enum in)
{
    for (auto entry : map) {
        if (entry && entry->first == in) {
            return entry->second;
        }
    }
    throw std::invalid_argument(fmt::format("invalid {} value {}", name, (int)in));
}

template <class Map, class Enum>
auto enum_to_rpc(const char* name, const Map& map, Enum in)
{
    for (auto entry : map) {
        if (entry && entry->second == in) {
            return entry->first;
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

void device_info_from_rpc(DeviceInfo& out, const PrDeviceInfo& in)
{
    // type
    switch (in.type()) {
    case PR_DEVICE_TYPE_SENDER:
        out.type = DeviceType::Sender;

        if (!in.has_sender_config()) {
            throw std::invalid_argument(
                "sender config should be present for sender device");
        }
        if (in.has_receiver_config()) {
            throw std::invalid_argument(
                "receiver config should not be present for sender device");
        }

        break;

    case PR_DEVICE_TYPE_RECEIVER:
        out.type = DeviceType::Receiver;

        if (in.has_sender_config()) {
            throw std::invalid_argument(
                "sender config should not be present for sender device");
        }
        if (!in.has_receiver_config()) {
            throw std::invalid_argument(
                "receiver config should be present for sender device");
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
    }

    if (in.local_config().has_channel_set()) {
        out.local_config.channel_set = enum_from_rpc(
            "channel_set", channel_set_map, in.local_config().channel_set());
    }

    // sender config
    if (in.has_sender_config()) {
        out.sender_config = SenderConfig {};

        if (in.sender_config().has_packet_encoding()) {
            out.sender_config->packet_encoding = enum_from_rpc("packet_encoding",
                packet_encoding_map,
                in.sender_config().packet_encoding());
        }

        if (in.sender_config().has_packet_length()) {
            out.sender_config->packet_length_ns =
                nanoseconds_from_rpc("packet_length", in.sender_config().packet_length());
        }

        if (in.sender_config().has_fec_encoding()) {
            out.sender_config->fec_encoding = enum_from_rpc(
                "fec_encoding", fec_encoding_map, in.sender_config().fec_encoding());
        }

        if (in.sender_config().has_fec_block_source_packets()) {
            out.sender_config->fec_block_source_packets =
                in.sender_config().fec_block_source_packets();
        }

        if (in.sender_config().has_fec_block_repair_packets()) {
            out.sender_config->fec_block_repair_packets =
                in.sender_config().fec_block_repair_packets();
        }
    }

    // receiver config
    if (in.has_receiver_config()) {
        out.receiver_config = ReceiverConfig {};

        if (in.receiver_config().has_target_latency()) {
            out.receiver_config->target_latency_ns = nanoseconds_from_rpc(
                "target_latency", in.receiver_config().target_latency());
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
    out.mutable_local_config()->set_channel_set(
        enum_to_rpc("channel_set", channel_set_map, in.local_config.channel_set));

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
}

} // namespace rocvad
