/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_base.hpp"

#include <cassert>

using namespace rocvad;

void CmdBase::register_command(CLI::App* command)
{
    assert(!command_);
    assert(command);

    command_ = command;
}

bool CmdBase::parsed()
{
    assert(command_);

    return command_->parsed();
}
