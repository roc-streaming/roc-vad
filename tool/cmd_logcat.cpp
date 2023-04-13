/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_logcat.hpp"
#include "connector.hpp"

#include <google/protobuf/util/time_util.h>
#include <spdlog/spdlog.h>

#include <cassert>
#include <chrono>
#include <format>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

using namespace rocvad;

namespace {

std::string map_time(google::protobuf::Timestamp timestamp)
{
    const int64_t nanoseconds =
        google::protobuf::util::TimeUtil::TimestampToNanoseconds(timestamp);

    const auto time_point = std::chrono::system_clock::time_point(
        std::chrono::duration_cast<std::chrono::system_clock::duration>(
            std::chrono::nanoseconds(nanoseconds)));

    std::time_t c_time = std::chrono::system_clock::to_time_t(time_point);

    const uint64_t c_milliseconds =
        uint64_t(std::chrono::duration_cast<std::chrono::milliseconds>(
                     time_point.time_since_epoch())
                     .count() %
                 1000);

    std::stringstream ss;

    ss << std::put_time(std::localtime(&c_time), "%H:%M:%S.") << std::setfill('0')
       << std::setw(3) << c_milliseconds;

    return ss.str();
}

const char* map_level(MesgLogEntry::Level level)
{
    switch (level) {
    case MesgLogEntry::CRIT:
        return "[CC]";

    case MesgLogEntry::ERROR:
        return "[EE]";

    case MesgLogEntry::WARN:
        return "[WW]";

    case MesgLogEntry::INFO:
        return "[II]";

    case MesgLogEntry::DEBUG:
        return "[DD]";

    default:
        break;
    }

    return "[TT]";
}

} // namespace

CmdLogcat::CmdLogcat(CLI::App& parent)
{
    register_command(
        parent.add_subcommand("logcat", "receive and print logs from driver"));
}

bool CmdLogcat::execute()
{
    bool has_tried = false, has_connected = false;

    for (;;) {
        if (has_tried) {
            disconnect_();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        const bool success = connect_();

        if (!success && !has_tried) {
            std::cout << "...waiting for connection to driver\n";
        }

        has_tried = true;

        if (!success) {
            continue;
        }

        if (has_connected) {
            std::cout << "...restored connection to driver\n";
        }

        has_connected = true;

        session_();

        std::cout << "...lost connection to driver\n";
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
    MesgNone request;

    assert(stub_);
    std::unique_ptr<grpc::ClientReader<MesgLogEntry>> stream_reader =
        stub_->stream_logs(&context, request);

    if (!stream_reader) {
        return;
    }

    for (;;) {
        MesgLogEntry entry;

        if (!stream_reader->Read(&entry)) {
            stream_reader->Finish();
            return;
        }

        std::cout << map_time(entry.time()) << " " << map_level(entry.level()) << " "
                  << entry.text() << "\n";
    }
}
