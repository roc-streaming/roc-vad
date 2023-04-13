/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "driver_protocol.hpp"
#include "log_sender.hpp"
#include "log_syslog.hpp"

#include <spdlog/async.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <mutex>

namespace rocvad {

// Holds default spdlog logger.
// Configures spdlog to send logs to two destinations:
//  - syslog
//  - dynamically attached and detached LogSender objects
class LogManager
{
public:
    LogManager();
    ~LogManager();

    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;

    std::shared_ptr<LogSender> attach_sender(
        grpc::ServerWriter<MesgLogEntry>& stream_writer);

    void detach_sender(std::shared_ptr<LogSender> sender_sink);

private:
    std::shared_ptr<spdlog::async_logger> logger_;

    std::shared_ptr<LogSyslog> syslog_sink_;
    std::shared_ptr<spdlog::sinks::dist_sink_mt> dist_sink_;
};

} // namespace rocvad
