/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "environment.hpp"

#include <CLI11.hpp>

namespace rocvad {

// Base class for all commands.
class CmdBase
{
public:
    CmdBase() = default;
    virtual ~CmdBase() = default;

    CmdBase(const CmdBase&) = delete;
    CmdBase& operator=(const CmdBase&) = delete;

    virtual bool parsed();
    virtual bool execute(const Environment& env) = 0;

protected:
    void register_command(CLI::App* command);

private:
    CLI::App* command_ = nullptr;
};

} // namespace rocvad
