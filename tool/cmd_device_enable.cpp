/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_device_enable.hpp"
#include "connector.hpp"
#include "format.hpp"
#include "parse.hpp"
#include "print.hpp"

#include <spdlog/spdlog.h>

using namespace rocvad;

CmdDeviceEnable::CmdDeviceEnable(CLI::App& parent)
{
    auto command =
        parent.add_subcommand("enable", "Enable virtual device (activate and show)");

    command->add_flag("-u,--uid", use_uid_, "Select device by UID instead of index");
    command->add_option("index", index_or_uid_, "Device index (or UID of --uid is used)")
        ->required()
        ->expected(1);

    register_command(command);
}

bool CmdDeviceEnable::execute(const Environment& env)
{
    if (!use_uid_ && !parse_index(index_or_uid_, index_)) {
        return false;
    }

    Connector conn;

    auto stub = conn.connect();
    if (!stub) {
        return false;
    }

    spdlog::debug("sending toggle_device command");

    grpc::ClientContext context;
    rvpb::RvToggleRequest request;
    rvpb::RvNone response;

    if (use_uid_) {
        request.mutable_device()->set_uid(index_or_uid_);
    } else {
        request.mutable_device()->set_index(index_);
    }

    request.set_enabled(true);

    const grpc::Status status = stub->toggle_device(&context, request, &response);

    if (!status.ok()) {
        spdlog::error("failed to enable device: {}", status.error_message());
        return false;
    }

    if (use_uid_) {
        fmt::println("enabled device with uid \"{}\"", index_or_uid_);
    } else {
        fmt::println("enabled device with index {}", index_);
    }

    return true;
}
