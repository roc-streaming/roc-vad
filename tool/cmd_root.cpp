/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_root.hpp"
#include "cmd_device.hpp"
#include "cmd_info.hpp"
#include "cmd_logcat.hpp"

using namespace rocvad;

CmdRoot::CmdRoot(CLI::App& parent)
{
    register_command(&parent);

    register_subcommand(std::make_shared<CmdInfo>(parent));
    register_subcommand(std::make_shared<CmdLogcat>(parent));
    register_subcommand(std::make_shared<CmdDevice>(parent));
}
