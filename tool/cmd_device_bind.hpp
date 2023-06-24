/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "cmd_base.hpp"
#include "driver_protocol.hpp"

#include <cstdint>
#include <optional>
#include <string>

namespace rocvad {

class CmdDeviceBind : public CmdBase
{
public:
    CmdDeviceBind(CLI::App& parent);

    bool execute(const Environment& env) override;

private:
    bool send_command_(DriverProtocol::Stub* stub,
        const char* name,
        PrInterface interface,
        const std::string& uri);

    bool use_uid_ = false;
    std::string index_or_uid_;
    uint32_t index_ = 0;

    std::optional<uint32_t> slot_;

    std::optional<std::string> source_endpoint_;
    std::optional<std::string> repair_endpoint_;
    std::optional<std::string> control_endpoint_;
};

} // namespace rocvad
