/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "device_manager.hpp"
#include "driver_service.hpp"
#include "log_manager.hpp"

#include <aspl/Driver.hpp>
#include <aspl/Plugin.hpp>

#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>

#include <memory>

namespace rocvad {

// Top-level class, owns all other stuff.
class Driver
{
public:
    Driver();
    ~Driver();

    Driver(const Driver&) = delete;
    Driver& operator=(const Driver&) = delete;

    // Returns entry point reference for coreaudiod.
    AudioServerPlugInDriverRef reference();

private:
    // objects registerd in coreaudiod
    // aspl::Driver is object tree root and contains aspl::Plugin
    // aspl::Plugin contains all created devices
    std::shared_ptr<aspl::Driver> driver_;
    std::shared_ptr<aspl::Plugin> plugin_;

    // business logic
    std::shared_ptr<LogManager> log_manager_;
    std::shared_ptr<DeviceManager> device_manager_;

    // rpc implementation, invokes managers
    std::unique_ptr<DriverService> driver_service_;

    // grpc server, invokes methods of DriverService
    std::unique_ptr<grpc::Server> rpc_server_;
};

} // namespace rocvad
