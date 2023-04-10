/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "device_manager.hpp"
#include "build_info.hpp"

namespace rcp {

DeviceManager::DeviceManager(const std::shared_ptr<aspl::Plugin>& plugin)
    : plugin_(plugin)
{
}

grpc::Status DeviceManager::ping(grpc::ServerContext* context,
    const proto::None* request,
    proto::None* response)
{
    return grpc::Status::OK;
}

grpc::Status DeviceManager::get_info(grpc::ServerContext* context,
    const proto::None* request,
    proto::Info* response)
{
    std::unique_lock lock(device_mutex_);

    response->set_version(BuildInfo::version);
    response->set_commit(BuildInfo::commit);

    return grpc::Status::OK;
}

grpc::Status DeviceManager::add_device(grpc::ServerContext* context,
    const proto::AddDeviceArgs* request,
    proto::None* response)
{
    std::unique_lock lock(device_mutex_);

    devices_["TODO"] = std::make_shared<Device>(plugin_);

    return grpc::Status::OK;
}

grpc::Status DeviceManager::delete_device(grpc::ServerContext* context,
    const proto::DeleteDeviceArgs* request,
    proto::None* response)
{
    std::unique_lock lock(device_mutex_);

    devices_.erase("TODO");

    return grpc::Status::OK;
}

} // namespace rcp
