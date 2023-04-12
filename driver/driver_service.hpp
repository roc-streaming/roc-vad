/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "device_manager.hpp"
#include "driver_protocol.hpp"
#include "log_manager.hpp"

#include <memory>

namespace rocvad {

class DriverService : public DriverProtocol::Service
{
public:
    DriverService(std::shared_ptr<LogManager> log_manager,
        std::shared_ptr<DeviceManager> device_manager);

    DriverService(const DriverService&) = delete;
    DriverService& operator=(const DriverService&) = delete;

    grpc::Status ping(grpc::ServerContext* context,
        const MesgNone* request,
        MesgNone* response) override;

    grpc::Status get_info(grpc::ServerContext* context,
        const MesgNone* request,
        MesgInfo* response) override;

    grpc::Status stream_logs(grpc::ServerContext* context,
        const MesgNone* request,
        grpc::ServerWriter<MesgLogEntry>* writer) override;

    grpc::Status add_device(grpc::ServerContext* context,
        const MesgAddDevice* request,
        MesgNone* response) override;

    grpc::Status delete_device(grpc::ServerContext* context,
        const MesgDeleteDevice* request,
        MesgNone* response) override;

private:
    std::shared_ptr<LogManager> log_manager_;
    std::shared_ptr<DeviceManager> device_manager_;
};

} // namespace rocvad