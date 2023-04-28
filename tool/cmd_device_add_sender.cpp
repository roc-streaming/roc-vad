/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_device_add_sender.hpp"
#include "connector.hpp"
#include "parse.hpp"
#include "print.hpp"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

using namespace rocvad;

CmdDeviceAddSender::CmdDeviceAddSender(CLI::App& parent)
{
    auto command = parent.add_subcommand("sender", "Add sender virtual device");

    command->add_option("-u,--uid", uid_, "Device UID (omit to auto-generate)");
    command->add_option(
        "-n,--name", name_, "Human-readable device name (omit to auto-generate)");

    command->add_option(
        "-r,--rate", rate_, "Sample rate for virtual device, in hertz (e.g. 44100)");
    command->add_option("-c,--chans",
        chans_,
        fmt::format("Channel set for virtual device (supported values: {})",
            supported_enum_values(channel_set_map)));

    command->add_option("--packet-encoding",
        packet_encoding_,
        fmt::format("Sender packet encoding (supported values: {})",
            supported_enum_values(packet_encoding_map)));
    command->add_option("--packet-length",
        packet_length_,
        fmt::format("Sender packet length (number with one of the suffixes: {})",
            supported_duration_suffixes()));

    command->add_option("--fec",
        fec_encoding_,
        fmt::format("Sender FEC encoding (supported values: {})",
            supported_enum_values(fec_encoding_map)));
    command->add_option("--fec-nbsrc",
        fec_block_source_packets_,
        "Sender FEC block source packets count");
    command->add_option("--fec-nbrpr",
        fec_block_repair_packets_,
        "Sender FEC block repair packets count");

    register_command(command);
}

bool CmdDeviceAddSender::execute(const Environment& env)
{
    Connector conn;

    auto stub = conn.connect();
    if (!stub) {
        return false;
    }

    spdlog::debug("sending add_device command");

    grpc::ClientContext context;
    PrDeviceInfo request;
    PrDeviceInfo response;

    request.set_type(PR_DEVICE_TYPE_SENDER);

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
        PrChannelSet channel_set;
        if (!parse_enum("--chans", channel_set_map, *chans_, channel_set)) {
            return false;
        }
        request.mutable_local_config()->set_channel_set(channel_set);
    }

    if (packet_encoding_) {
        PrPacketEncoding packet_encoding;
        if (!parse_enum("--packet-encoding",
                packet_encoding_map,
                *packet_encoding_,
                packet_encoding)) {
            return false;
        }
        request.mutable_sender_config()->set_packet_encoding(packet_encoding);
    }

    if (packet_length_) {
        if (!parse_duration("--packet-length",
                *packet_length_,
                *request.mutable_sender_config()->mutable_packet_length())) {
            return false;
        }
    }

    if (fec_encoding_) {
        PrFecEncoding fec_encoding;
        if (!parse_enum("--fec", fec_encoding_map, *fec_encoding_, fec_encoding)) {
            return false;
        }
        request.mutable_sender_config()->set_fec_encoding(fec_encoding);
    }

    if (fec_block_source_packets_) {
        request.mutable_sender_config()->set_fec_block_source_packets(
            *fec_block_source_packets_);
    }

    if (fec_block_repair_packets_) {
        request.mutable_sender_config()->set_fec_block_repair_packets(
            *fec_block_repair_packets_);
    }

    const grpc::Status status = stub->add_device(&context, request, &response);

    if (!status.ok()) {
        spdlog::error("failed to add sender device: {}", status.error_message());
        return false;
    }

    print_device_info(response);

    return true;
}
