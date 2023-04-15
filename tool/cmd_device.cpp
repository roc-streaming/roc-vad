/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_device.hpp"
#include "cmd_device_add.hpp"
#include "cmd_device_delete.hpp"
#include "cmd_device_list.hpp"
#include "cmd_device_show.hpp"

using namespace rocvad;

CmdDevice::CmdDevice(CLI::App& parent)
{
    auto command = parent.add_subcommand("device", "device manipulation commands");

    register_command(command);

    register_subcommand(std::make_shared<CmdDeviceList>(*command));
    register_subcommand(std::make_shared<CmdDeviceShow>(*command));
    register_subcommand(std::make_shared<CmdDeviceAdd>(*command));
    register_subcommand(std::make_shared<CmdDeviceDelete>(*command));
}
