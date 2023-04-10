/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "device.hpp"
#include "device_manager_protocol.hpp"

#include <aspl/Plugin.hpp>

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace rcp {

class DeviceManager : public proto::DeviceManagerProtocol::Service
{
public:
    DeviceManager(const std::shared_ptr<aspl::Plugin>& plugin);

    DeviceManager(const DeviceManager&) = delete;
    DeviceManager& operator=(const DeviceManager&) = delete;

    grpc::Status ping(grpc::ServerContext* context,
        const proto::None* request,
        proto::None* response) override;

    grpc::Status get_info(grpc::ServerContext* context,
        const proto::None* request,
        proto::Info* response) override;

    grpc::Status add_device(grpc::ServerContext* context,
        const proto::AddDeviceArgs* request,
        proto::None* response) override;

    grpc::Status delete_device(grpc::ServerContext* context,
        const proto::DeleteDeviceArgs* request,
        proto::None* response) override;

private:
    std::shared_ptr<aspl::Plugin> plugin_;

    std::mutex device_mutex_;
    std::unordered_map<std::string, std::shared_ptr<Device>> devices_;
};

} // namespace rcp
