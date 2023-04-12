/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "log_syslog.hpp"

#include <syslog.h>

namespace rocvad {

namespace {

int level_to_prio(spdlog::level::level_enum level)
{
    switch (level) {
    case spdlog::level::critical:
    case spdlog::level::err:
        return LOG_ERR;

    default:
        break;
    }

    return LOG_NOTICE;
}

} // namespace

void LogSyslog::sink_it_(const spdlog::details::log_msg& msg)
{
    formatter_->format(msg, buf_);

    syslog(level_to_prio(msg.level), "%.*s", (int)buf_.size(), buf_.data());
}

void LogSyslog::flush_()
{
}

} // namespace rocvad
