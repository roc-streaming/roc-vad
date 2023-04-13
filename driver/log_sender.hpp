/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "driver_protocol.hpp"

#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/spdlog.h>

#include <condition_variable>
#include <memory>
#include <mutex>

namespace rocvad {

class LogSender : public spdlog::sinks::base_sink<std::mutex>,
                  public std::enable_shared_from_this<LogSender>
{
public:
    LogSender(grpc::ServerWriter<MesgLogEntry>& stream_writer);

    LogSender(const LogSender&) = delete;
    LogSender& operator=(const LogSender&) = delete;

    void wait_client_disconnect();

protected:
    void sink_it_(const spdlog::details::log_msg& msg) override;
    void flush_() override;

private:
    grpc::ServerWriter<MesgLogEntry>& stream_writer_;

    std::mutex mutex_;
    std::condition_variable cond_;
    bool stopped_ = false;

    spdlog::memory_buf_t buf_;
};

} // namespace rocvad
