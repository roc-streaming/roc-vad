/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "cmd_base.hpp"

#include <cstdint>
#include <string>

namespace rocvad {

class CmdDeviceShow : public CmdBase
{
public:
    CmdDeviceShow(CLI::App& parent);

    bool execute(const Environment& env) override;

private:
    bool use_uid_ = false;
    std::string index_or_uid_;
    uint32_t index_ = 0;
};

} // namespace rocvad
