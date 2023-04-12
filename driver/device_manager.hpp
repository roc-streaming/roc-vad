/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "device.hpp"
#include "driver_protocol.hpp"
#include "log_manager.hpp"

#include <aspl/Plugin.hpp>

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace rocvad {

class DeviceManager : public DriverProtocol::Service
{
public:
    DeviceManager(std::shared_ptr<LogManager> log_manager,
        std::shared_ptr<aspl::Plugin> plugin);

    DeviceManager(const DeviceManager&) = delete;
    DeviceManager& operator=(const DeviceManager&) = delete;

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
    std::shared_ptr<aspl::Plugin> plugin_;

    std::mutex device_mutex_;
    std::unordered_map<std::string, std::shared_ptr<Device>> devices_;
};

} // namespace rocvad
