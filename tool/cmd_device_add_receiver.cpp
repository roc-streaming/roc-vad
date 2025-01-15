/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_device_add_receiver.hpp"
#include "connector.hpp"
#include "parse.hpp"
#include "print.hpp"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

using namespace rocvad;

CmdDeviceAddReceiver::CmdDeviceAddReceiver(CLI::App& parent)
{
    auto command = parent.add_subcommand("receiver", "Add receiver virtual device");

    command->add_option("-u,--uid", device_uid_, "Device UID (omit to auto-generate)");
    command->add_option(
        "-n,--name", device_name_, "Human-readable device name (omit to auto-generate)");
    command->add_flag("--disabled", device_disabled_, "Create device in disabled state");

    // device_encoding
    auto device_encoding_opts = command->add_option_group("Device encoding");

    device_encoding_opts->add_option("-r,--device-rate",
        device_encoding_rate_,
        "Sample rate for virtual device, in hertz (e.g. 44100)");
    device_encoding_opts->add_option("-c,--device-chans",
        device_encoding_chans_,
        fmt::format("Channel set for virtual device (supported values: {})",
            supported_enum_values(channel_layout_map)));
    device_encoding_opts->add_option("-t,--device-tracks",
        device_encoding_tracks_,
        "Track count when using multitrack, in range 1 - 1024");
    device_encoding_opts->add_option("-b,--device-buffer",
        device_encoding_buffer_,
        fmt::format(
            "Buffer size for virtual device (number with one of the suffixes: {})",
            supported_duration_suffixes()));

    // packet_encoding
    auto packet_encoding_opts = command->add_option_group("Packet encoding");

    packet_encoding_opts->add_option("--packet-encoding-id",
        packet_encoding_id_,
        "Unique identifier to use for packet encoding");
    packet_encoding_opts->add_option("--packet-encoding-rate",
        packet_encoding_rate_,
        "Sample rate to use for packet encoding");
    packet_encoding_opts->add_option("--packet-encoding-format",
        packet_encoding_format_,
        fmt::format("Sample format to use for packet encoding (supported values: {})",
            supported_enum_values(sample_format_map)));
    packet_encoding_opts->add_option("--packet-encoding-chans",
        packet_encoding_chans_,
        fmt::format("Channel layout to use for packet encoding (supported values: {})",
            supported_enum_values(channel_layout_map)));
    packet_encoding_opts->add_option("--packet-encoding-tracks",
        packet_encoding_tracks_,
        "Packet track count when using multitrack, in range 1 - 1024");

    // resampler
    auto resampler_opts = command->add_option_group("Resampler");

    resampler_opts->add_option("--resampler-backend",
        resampler_backend_,
        fmt::format("Resampler backend (supported values: {})",
            supported_enum_values(resampler_backend_map)));
    resampler_opts->add_option("--resampler-profile",
        resampler_profile_,
        fmt::format("Resampler profile (supported values: {})",
            supported_enum_values(resampler_profile_map)));

    // latency_tuner
    auto latency_tuner_opts = command->add_option_group("Latency tuner");

    latency_tuner_opts->add_option("--latency-backend",
        latency_tuner_backend_,
        fmt::format("Latency tuner backend (supported values: {})",
            supported_enum_values(latency_tuner_backend_map)));
    latency_tuner_opts->add_option("--latency-profile",
        latency_tuner_profile_,
        fmt::format("Latency tuner profile (supported values: {})",
            supported_enum_values(latency_tuner_profile_map)));

    // latency
    auto latency_opts = command->add_option_group("Latency");

    latency_opts->add_option("--target-latency",
        target_latency_,
        fmt::format("Target latency (number with one of the suffixes: {})",
            supported_duration_suffixes()));
    latency_opts->add_option("--latency-tolerance",
        latency_tolerance_,
        "Maximum deviation of latency from target (same format)");

    // timeouts
    auto timeouts_opts = command->add_option_group("Timeouts");

    timeouts_opts->add_option("--no-play-timeout",
        no_playback_timeout_,
        fmt::format("No playback timeout (number with one of the suffixes: {})",
            supported_duration_suffixes()));
    timeouts_opts->add_option("--choppy-play-timeout",
        choppy_playback_timeout_,
        "Choppy playback timeout (same format)");

    register_command(command);
}

bool CmdDeviceAddReceiver::execute(const Environment& env)
{
    Connector conn;

    auto stub = conn.connect();
    if (!stub) {
        return false;
    }

    spdlog::debug("sending add_device command");

    grpc::ClientContext context;
    rvpb::RvDeviceInfo request;
    rvpb::RvDeviceInfo response;

    // device
    request.set_type(rvpb::RV_DEVICE_TYPE_RECEIVER);

    if (device_uid_) {
        request.set_uid(*device_uid_);
    }
    if (device_name_) {
        request.set_name(*device_name_);
    }
    if (device_disabled_) {
        request.set_enabled(!*device_disabled_);
    }

    // device_encoding
    if (device_encoding_rate_) {
        request.mutable_device_encoding()->set_sample_rate(*device_encoding_rate_);
    }
    if (device_encoding_chans_) {
        rvpb::RvChannelLayout channel_layout;
        if (!parse_enum("--device-chans",
                channel_layout_map,
                *device_encoding_chans_,
                channel_layout)) {
            return false;
        }
        request.mutable_device_encoding()->set_channel_layout(channel_layout);

        if (channel_layout == rvpb::RV_CHANNEL_LAYOUT_MULTITRACK) {
            if (device_encoding_tracks_) {
                request.mutable_device_encoding()->set_track_count(*device_encoding_tracks_);
            } else {
                return false;
            }
        }
    }
    if (device_encoding_buffer_) {
        if (!parse_duration("--device-buffer",
                *device_encoding_buffer_,
                *request.mutable_device_encoding()->mutable_buffer_length())) {
            return false;
        }
    }

    // packet_encoding
    if (packet_encoding_id_ || packet_encoding_rate_ || packet_encoding_format_ ||
        packet_encoding_chans_) {
        rvpb::RvPacketEncoding packet_encoding;

        if (packet_encoding_id_) {
            packet_encoding.set_encoding_id(*packet_encoding_id_);
        }
        if (packet_encoding_rate_) {
            packet_encoding.set_sample_rate(*packet_encoding_rate_);
        }
        if (packet_encoding_format_) {
            rvpb::RvSampleFormat sample_format;
            if (!parse_enum("--packet-encoding-format",
                    sample_format_map,
                    *packet_encoding_format_,
                    sample_format)) {
                return false;
            }
            packet_encoding.set_sample_format(sample_format);
        }
        if (packet_encoding_chans_) {
            rvpb::RvChannelLayout channel_layout;
            if (!parse_enum("--packet-encoding-chans",
                    channel_layout_map,
                    *packet_encoding_chans_,
                    channel_layout)) {
                return false;
            }
            packet_encoding.set_channel_layout(channel_layout);

            if (channel_layout == rvpb::RV_CHANNEL_LAYOUT_MULTITRACK) {
                if (packet_encoding_tracks_) {
                    packet_encoding.set_track_count(*packet_encoding_tracks_);
                } else {
                    return false;
                }
            }
        }

        *request.mutable_receiver_config()->add_packet_encodings() = packet_encoding;
    }

    // resampler
    if (resampler_backend_) {
        rvpb::RvResamplerBackend resampler_backend;
        if (!parse_enum("--resampler-backend",
                resampler_backend_map,
                *resampler_backend_,
                resampler_backend)) {
            return false;
        }
        request.mutable_receiver_config()->set_resampler_backend(resampler_backend);
    }
    if (resampler_profile_) {
        rvpb::RvResamplerProfile resampler_profile;
        if (!parse_enum("--resampler-profile",
                resampler_profile_map,
                *resampler_profile_,
                resampler_profile)) {
            return false;
        }
        request.mutable_receiver_config()->set_resampler_profile(resampler_profile);
    }

    // latency_tuner
    if (latency_tuner_backend_) {
        rvpb::RvLatencyTunerBackend latency_tuner_backend;
        if (!parse_enum("--latency-backend",
                latency_tuner_backend_map,
                *latency_tuner_backend_,
                latency_tuner_backend)) {
            return false;
        }
        request.mutable_receiver_config()->set_latency_tuner_backend(
            latency_tuner_backend);
    }
    if (latency_tuner_profile_) {
        rvpb::RvLatencyTunerProfile latency_tuner_profile;
        if (!parse_enum("--latency-profile",
                latency_tuner_profile_map,
                *latency_tuner_profile_,
                latency_tuner_profile)) {
            return false;
        }
        request.mutable_receiver_config()->set_latency_tuner_profile(
            latency_tuner_profile);
    }

    // latency
    if (target_latency_) {
        if (!parse_duration("--target-latency",
                *target_latency_,
                *request.mutable_receiver_config()->mutable_target_latency())) {
            return false;
        }
    }
    if (latency_tolerance_) {
        if (!parse_duration("--latency-tolerance",
                *latency_tolerance_,
                *request.mutable_receiver_config()->mutable_latency_tolerance())) {
            return false;
        }
    }

    // timeouts
    if (no_playback_timeout_) {
        if (!parse_duration("--no-play-timeout",
                *no_playback_timeout_,
                *request.mutable_receiver_config()->mutable_no_playback_timeout())) {
            return false;
        }
    }
    if (choppy_playback_timeout_) {
        if (!parse_duration("--choppy-play-timeout",
                *choppy_playback_timeout_,
                *request.mutable_receiver_config()->mutable_choppy_playback_timeout())) {
            return false;
        }
    }

    const grpc::Status status = stub->add_device(&context, request, &response);

    if (!status.ok()) {
        spdlog::error("failed to add sender device: {}", status.error_message());
        return false;
    }

    print_device_info(response);

    return true;
}
