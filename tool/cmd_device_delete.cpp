/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_device_delete.hpp"
#include "connector.hpp"

#include <spdlog/spdlog.h>

using namespace rocvad;

CmdDeviceDelete::CmdDeviceDelete(CLI::App& parent)
{
    register_command(parent.add_subcommand("del", "delete virtual device"));
}

bool CmdDeviceDelete::execute(const Environment& env)
{
    Connector conn;

    auto stub = conn.connect();
    if (!stub) {
        return false;
    }

    spdlog::debug("sending delete_device command");

    grpc::ClientContext context;
    MesgDeleteDevice request;
    MesgNone response;

    const grpc::Status status = stub->delete_device(&context, request, &response);

    if (!status.ok()) {
        spdlog::error("failed to remove device");
        return false;
    }

    return true;
}
