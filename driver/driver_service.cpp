/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "driver_service.hpp"
#include "build_info.hpp"
#include "rpc_serdes.hpp"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <cassert>
#include <stdexcept>

namespace rocvad {

DriverService::DriverService(std::shared_ptr<LogManager> log_manager,
    std::shared_ptr<DeviceManager> device_manager)
    : log_manager_(log_manager)
    , device_manager_(device_manager)
{
    assert(log_manager_);
    assert(device_manager_);
}

grpc::Status DriverService::ping(grpc::ServerContext* context,
    const rvpb::RvNone* request,
    rvpb::RvNone* response)
{
    return execute_command_("ping", [=]() {
        // no-op
    });
}

grpc::Status DriverService::driver_info(grpc::ServerContext* context,
    const rvpb::RvNone* request,
    rvpb::RvDriverInfo* response)
{
    return execute_command_("driver_info", [=]() {
        response->set_version(BuildInfo::git_version);
        response->set_commit(BuildInfo::git_commit);
    });
}

grpc::Status DriverService::stream_logs(grpc::ServerContext* context,
    const rvpb::RvNone* request,
    grpc::ServerWriter<rvpb::RvLogEntry>* stream_writer)
{
    return execute_command_("stream_logs", [=]() {
        auto log_streamer = log_manager_->attach_streamer(*stream_writer);

        // log_streamer receives logs from spdlog and writes them to client stream
        // here we wait until log_streamer reports that client stream is closed
        log_streamer->wait_client_disconnect();

        log_manager_->detach_streamer(log_streamer);
    });
}

grpc::Status DriverService::get_all_devices(grpc::ServerContext* context,
    const rvpb::RvNone* request,
    rvpb::RvDeviceList* response)
{
    return execute_command_("get_all_devices", [=]() {
        const auto devices = device_manager_->get_all_devices();

        device_list_to_rpc(*response, devices);
    });
}

grpc::Status DriverService::get_device(grpc::ServerContext* context,
    const rvpb::RvDeviceSelector* request,
    rvpb::RvDeviceInfo* response)
{
    return execute_command_("get_device", [=]() {
        DeviceInfo device_info;

        if (request->has_index()) {
            device_info = device_manager_->get_device(request->index());
        } else {
            device_info = device_manager_->get_device(request->uid());
        }

        device_info_to_rpc(*response, device_info);
    });
}

grpc::Status DriverService::add_device(grpc::ServerContext* context,
    const rvpb::RvDeviceInfo* request,
    rvpb::RvDeviceInfo* response)
{
    return execute_command_("add_device", [=]() {
        DeviceInfo device_info;
        device_info_from_rpc(device_info, *request);

        device_info = device_manager_->add_device(device_info);

        device_info_to_rpc(*response, device_info);
    });
}

grpc::Status DriverService::delete_device(grpc::ServerContext* context,
    const rvpb::RvDeviceSelector* request,
    rvpb::RvNone* response)
{
    return execute_command_("delete_device", [=]() {
        if (request->has_index()) {
            device_manager_->delete_device(request->index());
        } else {
            device_manager_->delete_device(request->uid());
        }
    });
}

grpc::Status DriverService::toggle_device(grpc::ServerContext* context,
    const rvpb::RvToggleRequest* request,
    rvpb::RvNone* response)
{
    return execute_command_("toggle_device", [=]() {
        if (request->device().has_index()) {
            device_manager_->toggle_device(request->device().index(), request->enabled());
        } else {
            device_manager_->toggle_device(request->device().uid(), request->enabled());
        }
    });
}

grpc::Status DriverService::bind(grpc::ServerContext* context,
    const rvpb::RvEndpointRequest* request,
    rvpb::RvEndpointInfo* response)
{
    return execute_command_("bind", [=]() {
        DeviceEndpointInfo endpoint_info;
        endpoint_info_from_rpc(endpoint_info, request->endpoint());

        if (request->device().has_index()) {
            endpoint_info =
                device_manager_->bind_device(request->device().index(), endpoint_info);
        } else {
            endpoint_info =
                device_manager_->bind_device(request->device().uid(), endpoint_info);
        }

        endpoint_info_to_rpc(*response, endpoint_info);
    });
}

grpc::Status DriverService::connect(grpc::ServerContext* context,
    const rvpb::RvEndpointRequest* request,
    rvpb::RvEndpointInfo* response)
{
    return execute_command_("connect", [=]() {
        DeviceEndpointInfo endpoint_info;
        endpoint_info_from_rpc(endpoint_info, request->endpoint());

        if (request->device().has_index()) {
            endpoint_info =
                device_manager_->connect_device(request->device().index(), endpoint_info);
        } else {
            endpoint_info =
                device_manager_->connect_device(request->device().uid(), endpoint_info);
        }

        endpoint_info_to_rpc(*response, endpoint_info);
    });
}

grpc::Status DriverService::execute_command_(const char* name, std::function<void()> func)
{
    spdlog::debug("running {} command", name);

    try {
        func();
        spdlog::debug("command {} succeeded", name);
        return grpc::Status::OK;
    }
    catch (std::invalid_argument& e) {
        spdlog::error(
            "command {} failed with INVALID_ARGUMENT error: {}", name, e.what());
        return grpc::Status(grpc::INVALID_ARGUMENT, e.what());
    }
    catch (std::runtime_error& e) {
        spdlog::error("command {} failed with INTERNAL error: {}", name, e.what());
        return grpc::Status(grpc::INTERNAL, e.what());
    }
    catch (std::exception& e) {
        spdlog::error("command {} failed with UNKNOWN error: {}", name, e.what());
        return grpc::Status(grpc::UNKNOWN, e.what());
    }
}

} // namespace rocvad
