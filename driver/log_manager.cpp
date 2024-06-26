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

namespace {

class asplLogger : public aspl::Tracer
{
public:
    asplLogger()
        : aspl::Tracer(Mode::Custom, Style::Hierarchical)
    {
    }

protected:
    bool ShouldIgnore(const Operation& operation) override
    {
        return operation.Flags & Flags::Readonly;
    }

    void Print(const char* message) override
    {
        spdlog::trace("{}", std::string(message));
    }
};

void rocLogger(const roc_log_message* msg, void* arg)
{
    spdlog::level::level_enum level = spdlog::level::off;

    switch (msg->level) {
    case ROC_LOG_ERROR:
        level = spdlog::level::err;
        break;

    case ROC_LOG_INFO:
        level = spdlog::level::info;
        break;

    default:
        level = spdlog::level::debug;
        break;
    }

    spdlog::log(level, "{}: {}", std::string(msg->module), std::string(msg->text));
}

} // namespace

LogManager::LogManager()
{
    spdlog::init_thread_pool(10000, 1);

    syslog_sink_ = std::make_shared<LogSyslog>();
    syslog_sink_->set_pattern("[%L%L] %v");

    dist_sink_ = std::make_shared<spdlog::sinks::dist_sink_mt>();

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

std::shared_ptr<aspl::Tracer> LogManager::aspl_logger()
{
    return std::make_shared<asplLogger>();
}

roc_log_handler LogManager::roc_logger()
{
    return &rocLogger;
}

std::shared_ptr<LogStreamer> LogManager::attach_streamer(
    grpc::ServerWriter<rvpb::RvLogEntry>& stream_writer)
{
    spdlog::debug("attaching log streamer to dist sink");

    auto sender_sink = std::make_shared<LogStreamer>(stream_writer);
    sender_sink->set_pattern("%v");

    dist_sink_->add_sink(sender_sink);

    return sender_sink;
}

void LogManager::detach_streamer(std::shared_ptr<LogStreamer> sender_sink)
{
    spdlog::debug("detaching log streamer from dist sink");

    assert(sender_sink);
    dist_sink_->remove_sink(sender_sink);
}

} // namespace rocvad
