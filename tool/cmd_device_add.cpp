/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_device_add.hpp"
#include "connector.hpp"

#include <spdlog/spdlog.h>

using namespace rocvad;

CmdDeviceAdd::CmdDeviceAdd(CLI::App& parent)
{
    register_command(parent.add_subcommand("add", "add new virtual device"));
}

bool CmdDeviceAdd::execute()
{
    Connector conn;

    auto stub = conn.connect();
    if (!stub) {
        return false;
    }

    spdlog::debug("sending add_device command");

    grpc::ClientContext context;
    MesgAddDevice request;
    MesgNone response;

    const grpc::Status status = stub->add_device(&context, request, &response);

    if (!status.ok()) {
        spdlog::error("failed to add device");
        return false;
    }

    return true;
}
