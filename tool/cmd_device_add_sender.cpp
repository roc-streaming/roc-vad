/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_device_add_sender.hpp"
#include "connector.hpp"
#include "print.hpp"

#include <spdlog/spdlog.h>

using namespace rocvad;

CmdDeviceAddSender::CmdDeviceAddSender(CLI::App& parent)
{
    auto command = parent.add_subcommand("sender", "add sender virtual device");

    command->add_option("-u,--uid", uid_, "Device UID (omit to auto-generate)");
    command->add_option(
        "-n,--name", name_, "Human-readable device name (omit to auto-generate)");

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
    request.set_uid(uid_);
    request.set_name(name_);

    const grpc::Status status = stub->add_device(&context, request, &response);

    if (!status.ok()) {
        spdlog::error("failed to add sender device: {}", status.error_message());
        return false;
    }

    print_device_info(response);

    return true;
}
