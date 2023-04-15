/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "cmd_base.hpp"

namespace rocvad {

class CmdDeviceList : public CmdBase
{
public:
    CmdDeviceList(CLI::App& parent);

    bool execute(const Environment& env) override;

private:
    bool show_ = false;
};

} // namespace rocvad