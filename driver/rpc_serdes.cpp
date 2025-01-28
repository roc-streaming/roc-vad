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

int64_t nanoseconds_from_rpc(const char* name, google::protobuf::Duration duration)
{
    return google::protobuf::util::TimeUtil::DurationToNanoseconds(duration);
}

google::protobuf::Duration nanoseconds_to_rpc(const char* name, int64_t nanoseconds)
{
    return google::protobuf::util::TimeUtil::NanosecondsToDuration(nanoseconds);
}

} // namespace

void device_list_from_rpc(std::vector<DeviceInfo>& out, const rvpb::RvDeviceList& in)
{
    out.clear();

    for (const auto& in_info : in.devices()) {
        DeviceInfo out_info;
        device_info_from_rpc(out_info, in_info);

        out.push_back(out_info);
    }
}

void device_list_to_rpc(rvpb::RvDeviceList& out, const std::vector<DeviceInfo>& in)
{
    for (const auto& in_info : in) {
        device_info_to_rpc(*out.add_devices(), in_info);
    }
}

void device_info_from_rpc(DeviceInfo& out, const rvpb::RvDeviceInfo& in)
{
    // type
    switch (in.type()) {
    case rvpb::RV_DEVICE_TYPE_SENDER:
        out.type = DeviceType::Sender;
        out.sender_config = DeviceSenderConfig {};

        if (in.has_receiver_config()) {
            throw std::invalid_argument(
                "RvDeviceInfo.receiver_config should not be present for SENDER device");
        }

        break;

    case rvpb::RV_DEVICE_TYPE_RECEIVER:
        out.type = DeviceType::Receiver;
        out.receiver_config = DeviceReceiverConfig {};

        if (in.has_sender_config()) {
            throw std::invalid_argument(
                "RvDeviceInfo.sender_config should not be present for RECEIVER device");
        }

        break;

    default:
        throw std::invalid_argument(
            fmt::format("invalid RvDeviceInfo.device_type {}", (int)in.type()));
    }

    // device
    if (in.has_index()) {
        if (in.index() == 0) {
            throw std::invalid_argument(
                "RvDeviceInfo.index should be either unset or non-zero");
        }
        out.index = in.index();
    }

    if (in.has_uid()) {
        if (in.uid().empty()) {
            throw std::invalid_argument(
                "RvDeviceInfo.uid should be either unset or non-empty");
        }
        out.uid = in.uid();
    }

    if (in.has_name()) {
        if (in.name().empty()) {
            throw std::invalid_argument(
                "RvDeviceInfo.name should be either unset or non-empty");
        }
        out.name = in.name();
    }

    if (in.has_enabled()) {
        out.enabled = in.enabled();
    }

    // device_encoding
    if (in.device_encoding().has_sample_rate() &&
        in.device_encoding().sample_rate() != 0) {
        out.device_encoding.sample_rate = in.device_encoding().sample_rate();
    }

    if (in.device_encoding().has_channel_layout()) {
        out.device_encoding.channel_layout =
            enum_from_rpc("RvDeviceEncoding.channel_layout",
                channel_layout_map,
                in.device_encoding().channel_layout());
    }

    if (in.device_encoding().has_track_count()) {
        out.device_encoding.channel_count = in.device_encoding().track_count();
    }

    if (in.device_encoding().has_buffer_length()) {
        out.device_encoding.buffer_length_ns = nanoseconds_from_rpc(
            "RvDeviceEncoding.buffer_length", in.device_encoding().buffer_length());
    }

    // sender_config
    if (in.has_sender_config()) {
        // packet_encoding
        if (in.sender_config().has_packet_encoding()) {
            DevicePacketEncoding out_encoding;
            packet_encoding_from_rpc(out_encoding, in.sender_config().packet_encoding());

            out.sender_config->packet_encoding = out_encoding;
        }

        // packet_length
        if (in.sender_config().has_packet_length()) {
            out.sender_config->packet_length_ns = nanoseconds_from_rpc(
                "RvSenderConfig.packet_length", in.sender_config().packet_length());
        }

        // packet_interleaving
        if (in.sender_config().has_packet_interleaving()) {
            out.sender_config->packet_interleaving =
                in.sender_config().packet_interleaving();
        }

        // fec_encoding
        if (in.sender_config().has_fec_encoding()) {
            out.sender_config->fec_encoding = enum_from_rpc("RvSenderConfig.fec_encoding",
                fec_encoding_map,
                in.sender_config().fec_encoding());
        }

        // fec_block_source_packets
        if (in.sender_config().has_fec_block_source_packets()) {
            out.sender_config->fec_block_source_packets =
                in.sender_config().fec_block_source_packets();
        }

        // fec_block_repair_packets
        if (in.sender_config().has_fec_block_repair_packets()) {
            out.sender_config->fec_block_repair_packets =
                in.sender_config().fec_block_repair_packets();
        }

        // latency_tuner_backend
        if (in.sender_config().has_latency_tuner_backend()) {
            out.sender_config->latency_tuner_backend =
                enum_from_rpc("RvSenderConfig.latency_tuner_backend",
                    latency_tuner_backend_map,
                    in.sender_config().latency_tuner_backend());
        }

        // latency_tuner_profile
        if (in.sender_config().has_latency_tuner_profile()) {
            out.sender_config->latency_tuner_profile =
                enum_from_rpc("RvSenderConfig.latency_tuner_profile",
                    latency_tuner_profile_map,
                    in.sender_config().latency_tuner_profile());
        }

        // resampler_backend
        if (in.sender_config().has_resampler_backend()) {
            out.sender_config->resampler_backend =
                enum_from_rpc("RvSenderConfig.resampler_backend",
                    resampler_backend_map,
                    in.sender_config().resampler_backend());
        }

        // resampler_profile
        if (in.sender_config().has_resampler_profile()) {
            out.sender_config->resampler_profile =
                enum_from_rpc("RvSenderConfig.resampler_profile",
                    resampler_profile_map,
                    in.sender_config().resampler_profile());
        }

        // target_latency
        if (in.sender_config().has_target_latency()) {
            out.sender_config->target_latency_ns = nanoseconds_from_rpc(
                "RvSenderConfig.target_latency", in.sender_config().target_latency());
        }

        // latency_tolerance
        if (in.sender_config().has_latency_tolerance()) {
            out.sender_config->latency_tolerance_ns =
                nanoseconds_from_rpc("RvSenderConfig.latency_tolerance",
                    in.sender_config().latency_tolerance());
        }
    }

    // receiver_config
    if (in.has_receiver_config()) {
        // packet_encodings
        for (const auto& in_encoding : in.receiver_config().packet_encodings()) {
            DevicePacketEncoding out_encoding;
            packet_encoding_from_rpc(out_encoding, in_encoding);

            out.receiver_config->packet_encodings.push_back(out_encoding);
        }

        // latency_tuner_backend
        if (in.receiver_config().has_latency_tuner_backend()) {
            out.receiver_config->latency_tuner_backend =
                enum_from_rpc("RvReceiverConfig.latency_tuner_backend",
                    latency_tuner_backend_map,
                    in.receiver_config().latency_tuner_backend());
        }

        // latency_tuner_profile
        if (in.receiver_config().has_latency_tuner_profile()) {
            out.receiver_config->latency_tuner_profile =
                enum_from_rpc("RvReceiverConfig.latency_tuner_profile",
                    latency_tuner_profile_map,
                    in.receiver_config().latency_tuner_profile());
        }

        // resampler_backend
        if (in.receiver_config().has_resampler_backend()) {
            out.receiver_config->resampler_backend =
                enum_from_rpc("RvReceiverConfig.resampler_backend",
                    resampler_backend_map,
                    in.receiver_config().resampler_backend());
        }

        // resampler_profile
        if (in.receiver_config().has_resampler_profile()) {
            out.receiver_config->resampler_profile =
                enum_from_rpc("RvReceiverConfig.resampler_profile",
                    resampler_profile_map,
                    in.receiver_config().resampler_profile());
        }

        // target_latency
        if (in.receiver_config().has_target_latency()) {
            out.receiver_config->target_latency_ns = nanoseconds_from_rpc(
                "RvReceiverConfig.target_latency", in.receiver_config().target_latency());
        }

        // latency_tolerance
        if (in.receiver_config().has_latency_tolerance()) {
            out.receiver_config->latency_tolerance_ns =
                nanoseconds_from_rpc("RvReceiverConfig.latency_tolerance",
                    in.receiver_config().latency_tolerance());
        }

        // no_playback_timeout
        if (in.receiver_config().has_no_playback_timeout()) {
            out.receiver_config->no_playback_timeout_ns =
                nanoseconds_from_rpc("RvReceiverConfig.no_playback_timeout",
                    in.receiver_config().no_playback_timeout());
        }

        // choppy_playback_timeout
        if (in.receiver_config().has_choppy_playback_timeout()) {
            out.receiver_config->choppy_playback_timeout_ns =
                nanoseconds_from_rpc("RvReceiverConfig.choppy_playback_timeout",
                    in.receiver_config().choppy_playback_timeout());
        }
    }

    // local_endpoints
    for (const auto& in_info : in.local_endpoints()) {
        DeviceEndpointInfo out_info;
        endpoint_info_from_rpc(out_info, in_info);

        out.local_endpoints.push_back(out_info);
    }

    // remote_endpoints
    for (const auto& in_info : in.remote_endpoints()) {
        DeviceEndpointInfo out_info;
        endpoint_info_from_rpc(out_info, in_info);

        out.remote_endpoints.push_back(out_info);
    }
}

void device_info_to_rpc(rvpb::RvDeviceInfo& out, const DeviceInfo& in)
{
    // type
    out.set_type(in.type == DeviceType::Sender ? rvpb::RV_DEVICE_TYPE_SENDER
                                               : rvpb::RV_DEVICE_TYPE_RECEIVER);

    // device
    out.set_index(in.index);
    out.set_uid(in.uid);
    out.set_name(in.name);
    out.set_enabled(in.enabled);

    // device_encoding
    out.mutable_device_encoding()->set_sample_rate(in.device_encoding.sample_rate);
    out.mutable_device_encoding()->set_channel_layout(
        enum_to_rpc("RvDeviceEncoding.channel_layout",
            channel_layout_map,
            in.device_encoding.channel_layout));
    if (in.device_encoding.channel_layout == ROC_CHANNEL_LAYOUT_MULTITRACK) {
        out.mutable_device_encoding()->set_track_count((uint32_t)in.device_encoding.channel_count);
    }

    *out.mutable_device_encoding()->mutable_buffer_length() = nanoseconds_to_rpc(
        "RvDeviceEncoding.buffer_length", in.device_encoding.buffer_length_ns);

    // sender_config
    if (in.type == DeviceType::Sender) {
        // ensure sender_config is always present for sender device
        out.mutable_sender_config();
    }
    if (in.sender_config) {
        // packet_encoding
        if (in.sender_config->packet_encoding) {
            packet_encoding_to_rpc(
                *out.mutable_sender_config()->mutable_packet_encoding(),
                *in.sender_config->packet_encoding);
        }

        // packet_length
        *out.mutable_sender_config()->mutable_packet_length() = nanoseconds_to_rpc(
            "RvSenderConfig.packet_length", in.sender_config->packet_length_ns);

        // packet_interleaving
        out.mutable_sender_config()->set_packet_interleaving(
            in.sender_config->packet_interleaving);

        // fec_encoding
        out.mutable_sender_config()->set_fec_encoding(
            enum_to_rpc("RvSenderConfig.fec_encoding",
                fec_encoding_map,
                in.sender_config->fec_encoding));

        // fec_block_source_packets
        out.mutable_sender_config()->set_fec_block_source_packets(
            in.sender_config->fec_block_source_packets);

        // fec_block_repair_packets
        out.mutable_sender_config()->set_fec_block_repair_packets(
            in.sender_config->fec_block_repair_packets);

        // latency_tuner_backend
        out.mutable_sender_config()->set_latency_tuner_backend(
            enum_to_rpc("RvSenderConfig.latency_tuner_backend",
                latency_tuner_backend_map,
                in.sender_config->latency_tuner_backend));

        // latency_tuner_profile
        out.mutable_sender_config()->set_latency_tuner_profile(
            enum_to_rpc("RvSenderConfig.latency_tuner_profile",
                latency_tuner_profile_map,
                in.sender_config->latency_tuner_profile));

        // resampler_backend
        out.mutable_sender_config()->set_resampler_backend(
            enum_to_rpc("RvSenderConfig.resampler_backend",
                resampler_backend_map,
                in.sender_config->resampler_backend));

        // resampler_profile
        out.mutable_sender_config()->set_resampler_profile(
            enum_to_rpc("RvSenderConfig.resampler_profile",
                resampler_profile_map,
                in.sender_config->resampler_profile));

        // target_latency
        *out.mutable_sender_config()->mutable_target_latency() = nanoseconds_to_rpc(
            "RvSenderConfig.target_latency", in.sender_config->target_latency_ns);

        // latency_tolerance
        *out.mutable_sender_config()->mutable_latency_tolerance() = nanoseconds_to_rpc(
            "RvSenderConfig.latency_tolerance", in.sender_config->latency_tolerance_ns);
    }

    // receiver_config
    if (in.type == DeviceType::Receiver) {
        // ensure receiver_config is always present for receiver device
        out.mutable_receiver_config();
    }
    if (in.receiver_config) {
        // packet_encodings
        for (const auto& in_encoding : in.receiver_config->packet_encodings) {
            rvpb::RvPacketEncoding out_encoding;
            packet_encoding_to_rpc(out_encoding, in_encoding);

            *out.mutable_receiver_config()->add_packet_encodings() = out_encoding;
        }

        // latency_tuner_backend
        out.mutable_receiver_config()->set_latency_tuner_backend(
            enum_to_rpc("RvReceiverConfig.latency_tuner_backend",
                latency_tuner_backend_map,
                in.receiver_config->latency_tuner_backend));

        // latency_tuner_profile
        out.mutable_receiver_config()->set_latency_tuner_profile(
            enum_to_rpc("RvReceiverConfig.latency_tuner_profile",
                latency_tuner_profile_map,
                in.receiver_config->latency_tuner_profile));

        // resampler_backend
        out.mutable_receiver_config()->set_resampler_backend(
            enum_to_rpc("RvReceiverConfig.resampler_backend",
                resampler_backend_map,
                in.receiver_config->resampler_backend));

        // resampler_profile
        out.mutable_receiver_config()->set_resampler_profile(
            enum_to_rpc("RvReceiverConfig.resampler_profile",
                resampler_profile_map,
                in.receiver_config->resampler_profile));

        // target_latency
        *out.mutable_receiver_config()->mutable_target_latency() = nanoseconds_to_rpc(
            "RvReceiverConfig.target_latency", in.receiver_config->target_latency_ns);

        // latency_tolerance
        *out.mutable_receiver_config()->mutable_latency_tolerance() =
            nanoseconds_to_rpc("RvReceiverConfig.latency_tolerance",
                in.receiver_config->latency_tolerance_ns);

        // no_playback_timeout
        *out.mutable_receiver_config()->mutable_no_playback_timeout() =
            nanoseconds_to_rpc("RvReceiverConfig.no_playback_timeout",
                in.receiver_config->no_playback_timeout_ns);

        // choppy_playback_timeout
        *out.mutable_receiver_config()->mutable_choppy_playback_timeout() =
            nanoseconds_to_rpc("RvReceiverConfig.choppy_playback_timeout",
                in.receiver_config->choppy_playback_timeout_ns);
    }

    // local_endpoints
    for (const auto& in_info : in.local_endpoints) {
        rvpb::RvEndpointInfo out_info;
        endpoint_info_to_rpc(out_info, in_info);

        *out.add_local_endpoints() = out_info;
    }

    // remote_endpoints
    for (const auto& in_info : in.remote_endpoints) {
        rvpb::RvEndpointInfo out_info;
        endpoint_info_to_rpc(out_info, in_info);

        *out.add_remote_endpoints() = out_info;
    }
}

void packet_encoding_from_rpc(DevicePacketEncoding& out, const rvpb::RvPacketEncoding& in)
{
    auto enc_id = in.encoding_id();
    if (enc_id <= 0 || enc_id >= 255) {
        throw std::invalid_argument(
            "RvPacketEncoding.encoding_id should be in range [1; 255]");
    }

    out.id = (roc_packet_encoding)enc_id;

    out.spec.rate = in.sample_rate();
    if (out.spec.rate == 0) {
        throw std::invalid_argument("RvPacketEncoding.sample_rate should not be zero");
    }

    out.spec.format = enum_from_rpc(
        "RvPacketEncoding.sample_format", sample_format_map, in.sample_format());

    out.spec.channels = enum_from_rpc(
        "RvPacketEncoding.channel_layout", channel_layout_map, in.channel_layout());

    if (out.spec.channels == ROC_CHANNEL_LAYOUT_MULTITRACK) {
        if (in.track_count() == 0){
            throw std::invalid_argument("For multi-track track count should be in range [1; 1000]");
        }
        out.spec.tracks = in.track_count();
    }
}

void packet_encoding_to_rpc(rvpb::RvPacketEncoding& out, const DevicePacketEncoding& in)
{
    out.set_encoding_id(in.id);
    out.set_sample_rate(in.spec.rate);
    out.set_sample_format(
        enum_to_rpc("RvPacketEncoding.sample_format", sample_format_map, in.spec.format));
    out.set_channel_layout(enum_to_rpc(
        "RvPacketEncoding.channel_layout", channel_layout_map, in.spec.channels));
    if (in.spec.channels == ROC_CHANNEL_LAYOUT_MULTITRACK) {
        if (in.spec.tracks == 0){
            throw std::invalid_argument("For multi-track track count should be in range [1; 1000]");
        }
        out.set_track_count(in.spec.tracks);
    }
}

void endpoint_info_from_rpc(DeviceEndpointInfo& out, const rvpb::RvEndpointInfo& in)
{
    if (in.has_slot()) {
        out.slot = in.slot();
    }

    out.interface =
        enum_from_rpc("RvEndpointInfo.interface", interface_map, in.interface());
    out.uri = in.uri();

    if (out.uri.empty()) {
        throw std::invalid_argument("RvEndpointInfo.uri should not be empty");
    }
}

void endpoint_info_to_rpc(rvpb::RvEndpointInfo& out, const DeviceEndpointInfo& in)
{
    out.set_slot(in.slot);
    out.set_interface(
        enum_to_rpc("RvEndpointInfo.interface", interface_map, in.interface));
    out.set_uri(in.uri);
}

} // namespace rocvad
