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

    command->add_option("-u,--uid", uid_, "Device UID (omit to auto-generate)");
    command->add_option(
        "-n,--name", name_, "Human-readable device name (omit to auto-generate)");

    command->add_option(
        "-r,--rate", rate_, "Sample rate for virtual device, in hertz (e.g. 44100)");
    command->add_option("-c,--chans",
        chans_,
        fmt::format("Channel set for virtual device (supported values: {})",
            supported_enum_values(channel_layout_map)));

    command->add_option("--target-latency",
        target_latency_,
        fmt::format("Receiver target latency (number with one of the suffixes: {})",
            supported_duration_suffixes()));

    command->add_option("--resampler-backend",
        resampler_backend_,
        fmt::format("Receiver resampler backend (supported values: {})",
            supported_enum_values(resampler_backend_map)));
    command->add_option("--resampler-profile",
        resampler_profile_,
        fmt::format("Receiver resampler profile (supported values: {})",
            supported_enum_values(resampler_profile_map)));

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

    request.set_type(rvpb::RV_DEVICE_TYPE_RECEIVER);

    if (uid_) {
        request.set_uid(*uid_);
    }

    if (name_) {
        request.set_name(*name_);
    }

    if (rate_) {
        request.mutable_local_config()->set_sample_rate(*rate_);
    }

    if (chans_) {
        rvpb::RvChannelLayout channel_layout;
        if (!parse_enum("--chans", channel_layout_map, *chans_, channel_layout)) {
            return false;
        }
        request.mutable_local_config()->set_channel_layout(channel_layout);
    }

    if (target_latency_) {
        if (!parse_duration("--target-latency",
                *target_latency_,
                *request.mutable_receiver_config()->mutable_target_latency())) {
            return false;
        }
    }

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

    const grpc::Status status = stub->add_device(&context, request, &response);

    if (!status.ok()) {
        spdlog::error("failed to add sender device: {}", status.error_message());
        return false;
    }

    print_device_info(response);

    return true;
}
