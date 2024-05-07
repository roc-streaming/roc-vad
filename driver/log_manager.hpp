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

#include <roc/log.h>
#include <spdlog/async.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/spdlog.h>
#include <aspl/Tracer.hpp>

#include <memory>
#include <mutex>

namespace rocvad {

// Driver log manager.
//
// Configures spdlog to send logs to two destinations:
//  - syslog
//  - dynamically attached and detached LogSender objects
//
// Configures libroc to writes logs to spdlog.
// Implements Tracer for libASPL that writes logs to spdlog.
class LogManager
{
public:
    LogManager();
    ~LogManager();

    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;

    // get tracer implementation to pass to libASPL
    std::shared_ptr<aspl::Tracer> aspl_logger();

    // get handler implementation to pass to roc-toolkit
    roc_log_handler roc_logger();

    // attach LogSender that duplicates all logs to gRPC stream
    std::shared_ptr<LogSender> attach_sender(
        grpc::ServerWriter<rvpb::RvLogEntry>& stream_writer);

    // detach LogSender
    void detach_sender(std::shared_ptr<LogSender> sender_sink);

private:
    std::shared_ptr<spdlog::async_logger> logger_;

    std::shared_ptr<LogSyslog> syslog_sink_;
    std::shared_ptr<spdlog::sinks::dist_sink_mt> dist_sink_;
};

} // namespace rocvad
