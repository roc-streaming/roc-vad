/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "driver_service.hpp"
#include "build_info.hpp"

#include <spdlog/spdlog.h>

#include <stdexcept>

namespace rocvad {

DriverService::DriverService(std::shared_ptr<LogManager> log_manager,
    std::shared_ptr<DeviceManager> device_manager)
    : log_manager_(log_manager)
    , device_manager_(device_manager)
{
}

grpc::Status DriverService::ping(grpc::ServerContext* context,
    const MesgNone* request,
    MesgNone* response)
{
    spdlog::debug("received ping command");

    return grpc::Status::OK;
}

grpc::Status DriverService::get_info(grpc::ServerContext* context,
    const MesgNone* request,
    MesgInfo* response)
{
    spdlog::debug("received get_info command");

    response->set_version(BuildInfo::version);
    response->set_commit(BuildInfo::commit);

    return grpc::Status::OK;
}

grpc::Status DriverService::stream_logs(grpc::ServerContext* context,
    const MesgNone* request,
    grpc::ServerWriter<MesgLogEntry>* writer)
{
    spdlog::debug("received stream_logs command");

    auto log_sender = log_manager_->attach_sender(*writer);

    log_sender->wait_client_disconnect();

    return grpc::Status::OK;
}

grpc::Status DriverService::add_device(grpc::ServerContext* context,
    const MesgAddDevice* request,
    MesgNone* response)
{
    spdlog::debug("received add_device command");

    try {
        device_manager_->add_device();
        return grpc::Status::OK;
    }
    catch (std::exception& e) {
        spdlog::error("failed to add device: %s", e.what());
        return grpc::Status(grpc::UNKNOWN, e.what());
    }
}

grpc::Status DriverService::delete_device(grpc::ServerContext* context,
    const MesgDeleteDevice* request,
    MesgNone* response)
{
    spdlog::debug("received delete_device command");

    try {
        device_manager_->delete_device();
        return grpc::Status::OK;
    }
    catch (std::exception& e) {
        spdlog::error("failed to add device: %s", e.what());
        return grpc::Status(grpc::UNKNOWN, e.what());
    }
}

} // namespace rocvad
