/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "cmd_compound.hpp"

namespace rcp {

class CmdDevice : public CmdCompound
{
public:
    CmdDevice(CLI::App& parent);
};

} // namespace rcp
