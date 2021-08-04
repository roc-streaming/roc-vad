/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "device_manager_protocol.hpp"

namespace rcp {

class Connector
{
public:
    Connector() = default;
    ~Connector();

    Connector(const Connector&) = delete;
    Connector& operator=(const Connector&) = delete;

    proto::DeviceManagerProtocol::Stub* connect();

    void disconnect();

private:
    std::unique_ptr<proto::DeviceManagerProtocol::Stub> stub_;
    std::shared_ptr<grpc::Channel> channel_;
};

} // namespace rcp
