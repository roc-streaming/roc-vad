/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "driver_protocol.hpp"

namespace rocvad {

class Connector
{
public:
    Connector(bool quiet = false);
    ~Connector();

    Connector(const Connector&) = delete;
    Connector& operator=(const Connector&) = delete;

    DriverProtocol::Stub* connect();

    void disconnect();

private:
    const bool quiet_;
    const std::string driver_socket_;

    std::unique_ptr<DriverProtocol::Stub> stub_;
    std::shared_ptr<grpc::Channel> channel_;
};

} // namespace rocvad
