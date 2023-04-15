/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_device_list.hpp"
#include "connector.hpp"
#include "print.hpp"

#include <spdlog/spdlog.h>

using namespace rocvad;

CmdDeviceList::CmdDeviceList(CLI::App& parent)
{
    auto command = parent.add_subcommand("list", "list virtual devices");

    command->add_flag("-s,--show", show_, "Show full device info");

    register_command(command);
}

bool CmdDeviceList::execute(const Environment& env)
{
    Connector conn;

    auto stub = conn.connect();
    if (!stub) {
        return false;
    }

    spdlog::debug("sending get_all_devices command");

    grpc::ClientContext context;
    MesgNone request;
    MesgDeviceList response;

    const grpc::Status status = stub->get_all_devices(&context, request, &response);

    if (!status.ok()) {
        spdlog::error("failed to get device list: {}", status.error_message());
        return false;
    }

    print_device_list(response, show_);

    return true;
}
