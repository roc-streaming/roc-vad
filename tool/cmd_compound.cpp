/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_compound.hpp"

#include <cassert>
#include <iostream>
#include <optional>

using namespace rcp;

void CmdCompound::register_subcommand(std::shared_ptr<rcp::CmdBase> subcommand)
{
    assert(subcommand);

    subcommands_.push_back(std::move(subcommand));
}

bool CmdCompound::parsed()
{
    assert(!subcommands_.empty());

    for (auto& cmd : subcommands_) {
        if (cmd->parsed()) {
            return true;
        }
    }

    return false;
}

bool CmdCompound::execute()
{
    assert(!subcommands_.empty());

    std::optional<bool> result;

    for (auto& cmd : subcommands_) {
        if (cmd->parsed()) {
            result = cmd->execute();
            break;
        }
    }

    if (!result) {
        std::cerr << "Missing sub-command\n";
        std::cerr << "Run with --help for more information.\n";

        result = false;
    }

    return *result;
}
