/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "driver_protocol.hpp"
#include "log_streamer.hpp"
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
//  - dynamically attached and detached LogStreamer objects
//
// Constructs libASPL Tracer that writes logs to spdlog.
// Constructs libroc log handler that writes logs to spdlog.
class LogManager
{
public:
    LogManager();
    ~LogManager();

    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;

    // get tracer implementation to pass to libASPL
    // returned tracer will write logs to spdlog
    std::shared_ptr<aspl::Tracer> aspl_logger();

    // get handler implementation to pass to roc-toolkit
    // returned handler will write logs to spdlog
    roc_log_handler roc_logger();

    // attach LogStreamer that duplicates all logs from spdlog to gRPC stream
    std::shared_ptr<LogStreamer> attach_streamer(
        grpc::ServerWriter<rvpb::RvLogEntry>& stream_writer);

    // detach LogStreamer
    void detach_streamer(std::shared_ptr<LogStreamer> sender_sink);

private:
    std::shared_ptr<spdlog::async_logger> logger_;

    std::shared_ptr<LogSyslog> syslog_sink_;
    std::shared_ptr<spdlog::sinks::dist_sink_mt> dist_sink_;
};

} // namespace rocvad
