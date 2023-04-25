/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "connector.hpp"
#include "plist_info.hpp"

#include <grpcpp/create_channel.h>
#include <spdlog/spdlog.h>

using namespace rocvad;

Connector::Connector(bool quiet)
    : quiet_(quiet)
    , driver_socket_(PlistInfo::driver_socket())
{
}

Connector::~Connector()
{
    disconnect();
}

DriverProtocol::Stub* Connector::connect()
{
    spdlog::info("trying to connect to driver at {}", driver_socket_);

    spdlog::debug("creating rpc channel");

    if (!(channel_ = grpc::CreateChannel(
              driver_socket_, grpc::InsecureChannelCredentials()))) {
        spdlog::log(quiet_ ? spdlog::level::info : spdlog::level::err,
            "can't connect to driver: failed to create rpc channel");
        disconnect();
        return {};
    }

    if (!(stub_ = DriverProtocol::NewStub(channel_))) {
        spdlog::log(quiet_ ? spdlog::level::info : spdlog::level::err,
            "can't connect to driver: failed to create rpc stub");
        disconnect();
        return {};
    }

    spdlog::debug("sending ping command");

    grpc::ClientContext context;
    MesgNone request;
    MesgNone response;

    const grpc::Status status = stub_->ping(&context, request, &response);

    if (!status.ok()) {
        spdlog::log(quiet_ ? spdlog::level::info : spdlog::level::err,
            "can't connect to driver: failed to ping rpc server: {}",
            status.error_message());
        disconnect();
        return {};
    }

    spdlog::info("successfully connected to driver");

    return stub_.get();
}

void Connector::disconnect()
{
    if (stub_ || channel_) {
        spdlog::info("disconnecting from driver");

        stub_.reset();
        channel_.reset();
    }
}
