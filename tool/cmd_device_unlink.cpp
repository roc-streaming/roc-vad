/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_device_unlink.hpp"
#include "connector.hpp"
#include "format.hpp"
#include "parse.hpp"
#include "print.hpp"

#include <spdlog/spdlog.h>

using namespace rocvad;

CmdDeviceUnlink::CmdDeviceUnlink(CLI::App& parent)
{
    auto command =
        parent.add_subcommand("unlink", "Unlink virtual device from remote endpoint(s)");

    command->add_flag("-u,--uid", use_uid_, "Select device by UID instead of index");
    command->add_option("index", index_or_uid_, "Device index (or UID of --uid is used)")
        ->required()
        ->expected(1);

    command->add_option("--slot", slot_, "Select slot to unlink from its endpoint");

    register_command(command);
}

bool CmdDeviceUnlink::execute(const Environment& env)
{
    if (!use_uid_ && !parse_index(index_or_uid_, index_)) {
        return false;
    }

    Connector conn;

    auto stub = conn.connect();
    if (!stub) {
        return false;
    }

    spdlog::debug("sending unlink_device command");

    grpc::ClientContext context;
    rvpb::RvUnlinkRequest request;
    rvpb::RvNone response;

    if (use_uid_) {
        request.mutable_device()->set_uid(index_or_uid_);
    } else {
        request.mutable_device()->set_index(index_);
    }

    if (slot_) {
        request.set_slot(*slot_);
    }

    const grpc::Status status = stub->unlink(&context, request, &response);

    if (!status.ok()) {
        spdlog::error("failed to unlink endpoint: {}", status.error_message());
        return false;
    }

    if (use_uid_) {
        fmt::println("unlinked device with uid \"{}\"", index_or_uid_);
    } else {
        fmt::println("unlinked device with index {}", index_);
    }

    return true;
}

