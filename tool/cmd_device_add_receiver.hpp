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
#include <optional>

namespace rocvad {

class CmdDeviceAddReceiver : public CmdBase
{
public:
    CmdDeviceAddReceiver(CLI::App& parent);

    bool execute(const Environment& env) override;

private:
    std::optional<std::string> uid_;
    std::optional<std::string> name_;

    std::optional<uint32_t> rate_;
    std::optional<std::string> chans_;

    std::optional<std::string> target_latency_;

    std::optional<std::string> resampler_backend_;
    std::optional<std::string> resampler_profile_;
};

} // namespace rocvad
