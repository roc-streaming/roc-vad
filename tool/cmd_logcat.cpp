/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_logcat.hpp"
#include "color_sink.hpp"
#include "connector.hpp"

#include <fmt/core.h>
#include <google/protobuf/util/time_util.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <cassert>
#include <chrono>
#include <string>
#include <thread>

using namespace rocvad;

namespace {

std::chrono::system_clock::time_point map_time(google::protobuf::Timestamp timestamp)
{
    const int64_t nanoseconds =
        google::protobuf::util::TimeUtil::TimestampToNanoseconds(timestamp);

    return std::chrono::system_clock::time_point(
        std::chrono::duration_cast<std::chrono::system_clock::duration>(
            std::chrono::nanoseconds(nanoseconds)));
}

spdlog::level::level_enum map_level(rvpb::RvLogEntry::Level level)
{
    switch (level) {
    case rvpb::RvLogEntry::CRIT:
        return spdlog::level::critical;

    case rvpb::RvLogEntry::ERROR:
        return spdlog::level::err;

    case rvpb::RvLogEntry::WARN:
        return spdlog::level::warn;

    case rvpb::RvLogEntry::INFO:
        return spdlog::level::info;

    case rvpb::RvLogEntry::DEBUG:
        return spdlog::level::debug;

    default:
        break;
    }

    return spdlog::level::trace;
}

} // namespace

CmdLogcat::CmdLogcat(CLI::App& parent)
{
    register_command(
        parent.add_subcommand("logcat", "Receive and print logs from driver"));
}

bool CmdLogcat::execute(const Environment& env)
{
    setup_(env);

    return run_();
}

void CmdLogcat::setup_(const Environment& env)
{
    logger_ = std::make_shared<spdlog::logger>(
        "logcat", std::make_shared<ColorSink>(stdout, env.color_mode));

    logger_->set_level(spdlog::level::trace);
    logger_->set_pattern("%^%H:%M:%S.%e [%L%L] %v%$");
}

bool CmdLogcat::run_()
{
    bool has_tried = false, has_connected = false;

    for (;;) {
        if (has_tried) {
            disconnect_();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        const bool success = connect_();

        if (!success && !has_tried) {
            fmt::println("...waiting for connection to driver");
        }

        has_tried = true;

        if (!success) {
            continue;
        }

        if (has_connected) {
            fmt::println("...restored connection to driver");
        }

        has_connected = true;

        session_();

        fmt::println("...lost connection to driver");
    }

    return true;
}

bool CmdLogcat::connect_()
{
    conn_ = std::make_unique<Connector>(true);
    stub_ = conn_->connect();

    return stub_ != nullptr;
}

void CmdLogcat::disconnect_()
{
    if (conn_) {
        conn_->disconnect();
    }

    conn_.reset();
}

void CmdLogcat::session_()
{
    spdlog::debug("sending stream_logs command");

    grpc::ClientContext context;
    rvpb::RvNone request;

    assert(stub_);
    std::unique_ptr<grpc::ClientReader<rvpb::RvLogEntry>> stream_reader =
        stub_->stream_logs(&context, request);

    if (!stream_reader) {
        return;
    }

    for (;;) {
        rvpb::RvLogEntry entry;

        if (!stream_reader->Read(&entry)) {
            stream_reader->Finish();
            return;
        }

        assert(logger_);
        logger_->log(map_time(entry.time()), {}, map_level(entry.level()), entry.text());
    }
}
