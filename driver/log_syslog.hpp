/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <spdlog/sinks/base_sink.h>

#include <mutex>

namespace rocvad {

class LogSyslog : public spdlog::sinks::base_sink<std::mutex>
{
public:
    LogSyslog() = default;

    LogSyslog(const LogSyslog&) = delete;
    LogSyslog& operator=(const LogSyslog&) = delete;

protected:
    void sink_it_(const spdlog::details::log_msg& msg) override;
    void flush_() override;

private:
    spdlog::memory_buf_t buf_;
};

} // namespace rocvad
