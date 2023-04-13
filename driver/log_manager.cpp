/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "log_manager.hpp"

#include <grpc/impl/codegen/log.h>

#include <cassert>
#include <iterator>

namespace rocvad {

LogManager::LogManager()
{
    spdlog::init_thread_pool(10000, 1);

    // we can't use spdlog::sinks::syslog_sink because it call openlog(), and
    // it's not correct to do it inside coreaudiod plugin
    syslog_sink_ = std::make_shared<LogSyslog>();
    syslog_sink_->set_level(spdlog::level::trace);
    syslog_sink_->set_pattern("[%L%L] %v");

    dist_sink_ = std::make_shared<spdlog::sinks::dist_sink_mt>();
    dist_sink_->set_level(spdlog::level::trace);

    auto sinks = {
        std::static_pointer_cast<spdlog::sinks::base_sink<std::mutex>>(syslog_sink_),
        std::static_pointer_cast<spdlog::sinks::base_sink<std::mutex>>(dist_sink_),
    };

    logger_ = std::make_shared<spdlog::async_logger>("default",
        std::begin(sinks),
        std::end(sinks),
        spdlog::thread_pool(),
        spdlog::async_overflow_policy::overrun_oldest);

    logger_->set_level(spdlog::level::trace);

    spdlog::set_default_logger(logger_);
}

LogManager::~LogManager()
{
    spdlog::shutdown();
}

std::shared_ptr<LogSender> LogManager::attach_sender(
    grpc::ServerWriter<MesgLogEntry>& stream_writer)
{
    spdlog::debug("attaching log sender to dist sink");

    auto sender_sink = std::make_shared<LogSender>(stream_writer);
    sender_sink->set_level(spdlog::level::trace);
    sender_sink->set_pattern("%v");

    dist_sink_->add_sink(sender_sink);

    return sender_sink;
}

void LogManager::detach_sender(std::shared_ptr<LogSender> sender_sink)
{
    spdlog::debug("detaching log sender from dist sink");

    assert(sender_sink);
    dist_sink_->remove_sink(sender_sink);
}

} // namespace rocvad
