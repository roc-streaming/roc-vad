/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "log_sender.hpp"

#include <google/protobuf/util/time_util.h>
#include <spdlog/spdlog.h>

#include <cassert>
#include <chrono>

namespace rocvad {

namespace {

google::protobuf::Timestamp map_time(std::chrono::system_clock::time_point time_point)
{
    const int64_t nanoseconds =
        int64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(
            time_point.time_since_epoch())
                    .count());

    return google::protobuf::util::TimeUtil::NanosecondsToTimestamp(nanoseconds);
}

PrLogEntry::Level map_level(spdlog::level::level_enum level)
{
    switch (level) {
    case spdlog::level::critical:
        return PrLogEntry::CRIT;

    case spdlog::level::err:
        return PrLogEntry::ERROR;

    case spdlog::level::warn:
        return PrLogEntry::WARN;

    case spdlog::level::info:
        return PrLogEntry::INFO;

    case spdlog::level::debug:
        return PrLogEntry::DEBUG;

    default:
        break;
    }

    return PrLogEntry::TRACE;
}

} // namespace

LogSender::LogSender(grpc::ServerWriter<PrLogEntry>& stream_writer)
    : stream_writer_(stream_writer)
{
}

void LogSender::wait_client_disconnect()
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

    buf_.clear();
    formatter_->format(msg, buf_);

    while (buf_.size() != 0 &&
           (buf_[buf_.size() - 1] == '\n' || buf_[buf_.size() - 1] == '\r')) {
        buf_.resize(buf_.size() - 1);
    }

    PrLogEntry entry;
    *entry.mutable_time() = map_time(msg.time);
    entry.set_level(map_level(msg.level));
    entry.set_text(std::string(buf_.begin(), buf_.end()));

    const bool result = stream_writer_.Write(entry);

    if (!result) {
        spdlog::debug("client log stream disconnected");

        std::unique_lock lock(mutex_);

        stopped_ = true;
        cond_.notify_all();
    }
}

void LogSender::flush_()
{
    // no-op
}

} // namespace rocvad
