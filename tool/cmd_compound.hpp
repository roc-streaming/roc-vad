/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "cmd_base.hpp"

#include <memory>
#include <vector>

namespace rocvad {

class CmdCompound : public CmdBase
{
public:
    bool parsed() override;
    bool execute() override;

protected:
    void register_subcommand(std::shared_ptr<CmdBase> subcommand);

private:
    std::vector<std::shared_ptr<CmdBase>> subcommands_;
};

} // namespace rocvad
