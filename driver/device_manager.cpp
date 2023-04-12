/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "device_manager.hpp"
#include "build_info.hpp"

#include <spdlog/spdlog.h>

namespace rocvad {

DeviceManager::DeviceManager(std::shared_ptr<LogManager> log_manager,
    std::shared_ptr<aspl::Plugin> plugin)
    : log_manager_(log_manager)
    , plugin_(plugin)
{
}

grpc::Status DeviceManager::ping(grpc::ServerContext* context,
    const MesgNone* request,
    MesgNone* response)
{
    // no lock

    spdlog::debug("received ping command");

    return grpc::Status::OK;
}

grpc::Status DeviceManager::get_info(grpc::ServerContext* context,
    const MesgNone* request,
    MesgInfo* response)
{
    // no lock

    spdlog::debug("received get_info command");

    response->set_version(BuildInfo::version);
    response->set_commit(BuildInfo::commit);

    return grpc::Status::OK;
}

grpc::Status DeviceManager::stream_logs(grpc::ServerContext* context,
    const MesgNone* request,
    grpc::ServerWriter<MesgLogEntry>* writer)
{
    // no lock

    spdlog::debug("received stream_logs command");

    auto log_sender = log_manager_->attach_sender(*writer);

    log_sender->wait_eof();

    return grpc::Status::OK;
}

grpc::Status DeviceManager::add_device(grpc::ServerContext* context,
    const MesgAddDevice* request,
    MesgNone* response)
{
    std::unique_lock device_lock(device_mutex_);

    spdlog::debug("received add_device command");

    devices_["TODO"] = std::make_shared<Device>(plugin_);

    return grpc::Status::OK;
}

grpc::Status DeviceManager::delete_device(grpc::ServerContext* context,
    const MesgDeleteDevice* request,
    MesgNone* response)
{
    std::unique_lock device_lock(device_mutex_);

    spdlog::debug("received delete_device command");

    devices_.erase("TODO");

    return grpc::Status::OK;
}

} // namespace rocvad
