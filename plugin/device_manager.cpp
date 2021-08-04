/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "device_manager.hpp"

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

grpc::Status DeviceManager::add_device(grpc::ServerContext* context,
    const proto::AddDeviceArgs* request,
    proto::None* response)
{
    return grpc::Status::OK;
}

grpc::Status DeviceManager::remove_device(grpc::ServerContext* context,
    const proto::RemoveDeviceArgs* request,
    proto::None* response)
{
    return grpc::Status::OK;
}

} // namespace rcp
