/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_device_show.hpp"
#include "connector.hpp"
#include "parse.hpp"
#include "print.hpp"

#include <spdlog/spdlog.h>

using namespace rocvad;

CmdDeviceShow::CmdDeviceShow(CLI::App& parent)
{
    auto command = parent.add_subcommand("show", "show virtual device info");

    command->add_flag("-u,--uid", use_uid_, "Select device by UID instead of index");
    command->add_option("index", index_or_uid_, "Device index (or UID of --uid is used)")
        ->required()
        ->expected(1);

    register_command(command);
}

bool CmdDeviceShow::execute(const Environment& env)
{
    index_t index = 0;

    if (!use_uid_ && !parse_index(index_or_uid_, index)) {
        return false;
    }

    Connector conn;

    auto stub = conn.connect();
    if (!stub) {
        return false;
    }

    spdlog::debug("sending get_device command");

    grpc::ClientContext context;
    PrDeviceSelector request;
    PrDeviceInfo response;

    if (use_uid_) {
        request.set_uid(index_or_uid_);
    } else {
        request.set_index(index);
    }

    const grpc::Status status = stub->get_device(&context, request, &response);

    if (!status.ok()) {
        spdlog::error("failed to get device info: {}", status.error_message());
        return false;
    }

    print_device_info(response);

    return true;
}
