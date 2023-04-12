/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "connector.hpp"

#include <grpcpp/create_channel.h>
#include <spdlog/spdlog.h>

using namespace rocvad;

Connector::~Connector()
{
    disconnect();
}

DriverProtocol::Stub* Connector::connect()
{
    // TODO: get address from Info.plist
    const std::string driver_address = "127.0.0.1:9712";

    spdlog::info("trying to connect to driver at {}", driver_address);

    spdlog::debug("creating rpc channel");

    if (!(channel_ = grpc::CreateChannel(
              driver_address, grpc::InsecureChannelCredentials()))) {
        spdlog::error("can't connect to driver: failed to create rpc channel");
        return {};
    }

    if (!(stub_ = DriverProtocol::NewStub(channel_))) {
        spdlog::error("can't connect to driver: failed to create rpc stub");
        return {};
    }

    spdlog::debug("sending ping command");

    grpc::ClientContext context;
    MesgNone request;
    MesgNone response;

    const grpc::Status status = stub_->ping(&context, request, &response);

    if (!status.ok()) {
        spdlog::error("can't connect to driver: failed to ping rpc server");
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
