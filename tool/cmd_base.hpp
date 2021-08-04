/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <CLI11.hpp>

namespace rcp {

class CmdBase
{
public:
    virtual ~CmdBase() = default;

    virtual bool parsed();
    virtual bool execute() = 0;

protected:
    void register_command(CLI::App* command);

private:
    CLI::App* command_ = nullptr;
};

} // namespace rcp
