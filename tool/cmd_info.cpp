/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_info.hpp"
#include "build_info.hpp"
#include "connector.hpp"

#include <spdlog/spdlog.h>

#include <iostream>

using namespace rocvad;

CmdInfo::CmdInfo(CLI::App& parent)
{
    register_command(parent.add_subcommand("info", "print driver and tool info"));
}

bool CmdInfo::execute()
{
    Connector conn;

    auto stub = conn.connect();
    if (!stub) {
        return false;
    }

    spdlog::debug("sending get_info command");

    grpc::ClientContext context;
    MesgNone request;
    MesgInfo response;

    const grpc::Status status = stub->get_info(&context, request, &response);

    if (!status.ok()) {
        spdlog::error("failed to get driver info");
        return false;
    }

    std::cout << "driver is loaded\n";

    std::cout << "\n";

    std::cout << "driver:\n";
    std::cout << "  version: " << response.version() << "\n";
    std::cout << "  commit:  " << response.commit().substr(0, 7) << "\n";

    std::cout << "\n";

    std::cout << "client:\n";
    std::cout << "  version: " << BuildInfo::version << "\n";
    std::cout << "  commit:  " << std::string(BuildInfo::commit).substr(0, 7) << "\n";

    return true;
}
