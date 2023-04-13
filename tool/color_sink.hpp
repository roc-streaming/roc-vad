/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <cstdio>
#include <mutex>

namespace rocvad {

// Log sink with custom color set.
class ColorSink : public spdlog::sinks::ansicolor_sink<spdlog::details::console_mutex>
{
public:
    ColorSink(FILE* target_file, spdlog::color_mode color_mode)
        : ansicolor_sink(target_file, color_mode)
    {
        set_color(spdlog::level::critical, red);
        set_color(spdlog::level::err, red);
        set_color(spdlog::level::warn, yellow);
        set_color(spdlog::level::info, green);
        set_color(spdlog::level::debug, reset);
        set_color(spdlog::level::trace, white);
    }
};

} // namespace rocvad
