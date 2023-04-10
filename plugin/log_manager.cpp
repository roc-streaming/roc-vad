/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "log_manager.hpp"

#include <iterator>

namespace rcp {

LogManager::LogManager()
{
    spdlog::init_thread_pool(10 * 1024, 1);

    syslog_sink_ = std::make_shared<LogSyslog>();
    dist_sink_ = std::make_shared<spdlog::sinks::dist_sink<std::mutex>>();

    auto sinks = {
        std::static_pointer_cast<spdlog::sinks::base_sink<std::mutex>>(syslog_sink_),
        std::static_pointer_cast<spdlog::sinks::base_sink<std::mutex>>(dist_sink_),
    };

    logger_ = std::make_shared<spdlog::async_logger>("default",
        std::begin(sinks),
        std::end(sinks),
        spdlog::thread_pool(),
        spdlog::async_overflow_policy::overrun_oldest);

    logger_->set_level(spdlog::level::debug);

    spdlog::set_default_logger(logger_);
}

LogManager::~LogManager()
{
    spdlog::shutdown();
}

std::shared_ptr<LogSender> LogManager::attach_sender(
    grpc::ServerWriter<proto::LogMessage>& stream_writer)
{
    return std::make_shared<LogSender>(dist_sink_, stream_writer);
}

} // namespace rcp
