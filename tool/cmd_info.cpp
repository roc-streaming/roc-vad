/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_info.hpp"
#include "build_info.hpp"
#include "connector.hpp"
#include "print.hpp"

#include <spdlog/spdlog.h>

using namespace rocvad;

CmdInfo::CmdInfo(CLI::App& parent)
{
    register_command(parent.add_subcommand("info", "print driver and tool info"));
}

bool CmdInfo::execute(const Environment& env)
{
    Connector conn;

    auto stub = conn.connect();
    if (!stub) {
        return false;
    }

    spdlog::debug("sending driver_info command");

    grpc::ClientContext context;
    MesgNone request;
    MesgDriverInfo response;

    const grpc::Status status = stub->driver_info(&context, request, &response);

    if (!status.ok()) {
        spdlog::error("failed to get driver info: {}", status.error_message());
        return false;
    }

    print_driver_and_client_info(response);

    return true;
}
