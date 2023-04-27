/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_compound.hpp"

#include <fmt/core.h>

#include <cassert>
#include <cstdio>
#include <optional>

using namespace rocvad;

void CmdCompound::register_subcommand(std::shared_ptr<CmdBase> subcommand)
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

bool CmdCompound::execute(const Environment& env)
{
    assert(!subcommands_.empty());

    std::optional<bool> result;

    for (auto& cmd : subcommands_) {
        if (cmd->parsed()) {
            result = cmd->execute(env);
            break;
        }
    }

    if (!result) {
        fmt::println(stderr, "Missing sub-command");
        fmt::println(stderr, "Run with --help for more information.");

        result = false;
    }

    return *result;
}
