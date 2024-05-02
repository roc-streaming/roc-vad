/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "device_manager.hpp"
#include "driver_protocol.hpp"
#include "log_manager.hpp"

#include <functional>
#include <memory>

namespace rocvad {

// Implements RPC methods defined by .proto file.
class DriverService : public rvpb::RvDriver::Service
{
public:
    DriverService(std::shared_ptr<LogManager> log_manager,
        std::shared_ptr<DeviceManager> device_manager);

    DriverService(const DriverService&) = delete;
    DriverService& operator=(const DriverService&) = delete;

    grpc::Status ping(grpc::ServerContext* context,
        const rvpb::RvNone* request,
        rvpb::RvNone* response) override;

    grpc::Status driver_info(grpc::ServerContext* context,
        const rvpb::RvNone* request,
        rvpb::RvDriverInfo* response) override;

    grpc::Status stream_logs(grpc::ServerContext* context,
        const rvpb::RvNone* request,
        grpc::ServerWriter<rvpb::RvLogEntry>* writer) override;

    grpc::Status get_all_devices(grpc::ServerContext* context,
        const rvpb::RvNone* request,
        rvpb::RvDeviceList* response) override;

    grpc::Status get_device(grpc::ServerContext* context,
        const rvpb::RvDeviceSelector* request,
        rvpb::RvDeviceInfo* response) override;

    grpc::Status add_device(grpc::ServerContext* context,
        const rvpb::RvDeviceInfo* request,
        rvpb::RvDeviceInfo* response) override;

    grpc::Status delete_device(grpc::ServerContext* context,
        const rvpb::RvDeviceSelector* request,
        rvpb::RvNone* response) override;

    grpc::Status toggle_device(grpc::ServerContext* context,
        const rvpb::RvToggleRequest* request,
        rvpb::RvNone* response) override;

    grpc::Status bind(grpc::ServerContext* context,
        const rvpb::RvEndpointRequest* request,
        rvpb::RvEndpointInfo* response) override;

    grpc::Status connect(grpc::ServerContext* context,
        const rvpb::RvEndpointRequest* request,
        rvpb::RvEndpointInfo* response) override;

private:
    grpc::Status execute_command_(const char* name, std::function<void()> func);

    std::shared_ptr<LogManager> log_manager_;
    std::shared_ptr<DeviceManager> device_manager_;
};

} // namespace rocvad
