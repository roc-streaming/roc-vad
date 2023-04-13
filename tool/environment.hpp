/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <spdlog/spdlog.h>

namespace rocvad {

// Command execution environment.
struct Environment
{
    spdlog::level::level_enum log_level = spdlog::level::err;

    spdlog::color_mode color_mode = spdlog::color_mode::automatic;
};

} // namespace rocvad
