/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "log_sender.hpp"

#include <spdlog/spdlog.h>

namespace rcp {

LogSender::LogSender(std::shared_ptr<spdlog::sinks::dist_sink<std::mutex>> dist_sink,
    grpc::ServerWriter<proto::LogMessage>& stream_writer)
    : dist_sink_(dist_sink)
    , stream_writer_(stream_writer)
{
    spdlog::debug("attaching log sender to sink");

    dist_sink_->add_sink(shared_from_this());
}

LogSender::~LogSender()
{
    spdlog::debug("detaching log sender from sink");

    dist_sink_->remove_sink(shared_from_this());
}

void LogSender::wait_eof()
{
    std::unique_lock lock(mutex_);

    while (!stopped_) {
        cond_.wait(lock);
    }
}

void LogSender::sink_it_(const spdlog::details::log_msg& msg)
{
    if (stopped_) {
        return;
    }

    formatter_->format(msg, buf_);

    proto::LogMessage stream_msg;
    stream_msg.set_text(std::string(buf_.begin(), buf_.end()));

    const bool result = stream_writer_.Write(stream_msg);

    if (!result) {
        spdlog::debug("client log stream disconnected");

        std::unique_lock lock(mutex_);

        stopped_ = true;
        cond_.notify_all();
    }
}

void LogSender::flush_()
{
}

} // namespace rcp
