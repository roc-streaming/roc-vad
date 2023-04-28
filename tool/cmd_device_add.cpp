/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_device_add.hpp"
#include "cmd_device_add_receiver.hpp"
#include "cmd_device_add_sender.hpp"

using namespace rocvad;

CmdDeviceAdd::CmdDeviceAdd(CLI::App& parent)
{
    auto command = parent.add_subcommand("add", "Add virtual device");

    register_command(command);

    register_subcommand(std::make_shared<CmdDeviceAddSender>(*command));
    register_subcommand(std::make_shared<CmdDeviceAddReceiver>(*command));
}
