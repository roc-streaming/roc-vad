/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_device_delete.hpp"
#include "connector.hpp"
#include "parse.hpp"

#include <spdlog/spdlog.h>

using namespace rocvad;

CmdDeviceDelete::CmdDeviceDelete(CLI::App& parent)
{
    auto command = parent.add_subcommand("del", "Delete virtual device");

    command->add_flag("-u,--uid", use_uid_, "Select device by UID instead of index");
    command->add_option("index", index_or_uid_, "Device index (or UID of --uid is used)")
        ->required()
        ->expected(1);

    register_command(command);
}

bool CmdDeviceDelete::execute(const Environment& env)
{
    if (!use_uid_ && !parse_index(index_or_uid_, index_)) {
        return false;
    }

    Connector conn;

    auto stub = conn.connect();
    if (!stub) {
        return false;
    }

    spdlog::debug("sending delete_device command");

    grpc::ClientContext context;
    PrDeviceSelector request;
    PrNone response;

    if (use_uid_) {
        request.set_uid(index_or_uid_);
    } else {
        request.set_index(index_);
    }

    const grpc::Status status = stub->delete_device(&context, request, &response);

    if (!status.ok()) {
        spdlog::error("failed to delete device: {}", status.error_message());
        return false;
    }

    if (use_uid_) {
        fmt::println("deleted device with uid \"{}\"", index_or_uid_);
    } else {
        fmt::println("deleted device with index {}", index_);
    }

    return true;
}
