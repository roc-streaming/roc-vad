/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "device_manager.hpp"

#include <aspl/Driver.hpp>

#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>

#include <memory>

namespace rcp {

class Driver
{
public:
    Driver();
    ~Driver();

    AudioServerPlugInDriverRef reference();

private:
    std::shared_ptr<aspl::Driver> driver_;

    std::unique_ptr<DeviceManager> deviceManager_;

    std::unique_ptr<grpc::Server> rpcServer_;
};

} // namespace rcp
