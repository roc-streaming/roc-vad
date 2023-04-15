/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "driver_service.hpp"
#include "build_info.hpp"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <cassert>
#include <stdexcept>

namespace rocvad {

namespace {

void device_config_from_rpc(DeviceConfig& out, const MesgDeviceConfig& in)
{
    out.type =
        in.type() == MesgDeviceConfig::SENDER ? DeviceType::Sender : DeviceType::Receiver;
    out.uid = in.uid();
    out.name = in.name();
}

void device_info_to_rpc(MesgDeviceInfo& out, const DeviceInfo& in)
{
    out.set_index(in.index);
    out.mutable_config()->set_type(in.config.type == DeviceType::Sender
                                       ? MesgDeviceConfig::SENDER
                                       : MesgDeviceConfig::RECEIVER);
    out.mutable_config()->set_uid(in.config.uid);
    out.mutable_config()->set_name(in.config.name);
}

} // namespace

DriverService::DriverService(std::shared_ptr<LogManager> log_manager,
    std::shared_ptr<DeviceManager> device_manager)
    : log_manager_(log_manager)
    , device_manager_(device_manager)
{
    assert(log_manager_);
    assert(device_manager_);
}

grpc::Status DriverService::ping(grpc::ServerContext* context,
    const MesgNone* request,
    MesgNone* response)
{
    return execute_command_("ping", [=]() {
        // no-op
    });
}

grpc::Status DriverService::driver_info(grpc::ServerContext* context,
    const MesgNone* request,
    MesgDriverInfo* response)
{
    return execute_command_("driver_info", [=]() {
        response->set_version(BuildInfo::version);
        response->set_commit(BuildInfo::commit);
    });
}

grpc::Status DriverService::stream_logs(grpc::ServerContext* context,
    const MesgNone* request,
    grpc::ServerWriter<MesgLogEntry>* writer)
{
    return execute_command_("stream_logs", [=]() {
        auto log_sender = log_manager_->attach_sender(*writer);

        // log_sender will receive logs and write them to client stream
        // we'll wait until client disconnects and next write to client fails
        log_sender->wait_client_disconnect();

        log_manager_->detach_sender(log_sender);
    });
}

grpc::Status DriverService::get_all_devices(grpc::ServerContext* context,
    const MesgNone* request,
    MesgDeviceList* response)
{
    return execute_command_("get_all_devices", [=]() {
        const auto devices = device_manager_->get_all_devices();

        for (const auto& device_info : devices) {
            device_info_to_rpc(*response->add_devices(), device_info);
        }
    });
}

grpc::Status DriverService::get_device(grpc::ServerContext* context,
    const MesgDeviceSelector* request,
    MesgDeviceInfo* response)
{
    return execute_command_("get_device", [=]() {
        const auto device_info = request->has_index()
                                     ? device_manager_->get_device(request->index())
                                     : device_manager_->get_device(request->uid());

        device_info_to_rpc(*response, device_info);
    });
}

grpc::Status DriverService::add_device(grpc::ServerContext* context,
    const MesgDeviceConfig* request,
    MesgDeviceInfo* response)
{
    return execute_command_("add_device", [=]() {
        DeviceConfig device_config;
        device_config_from_rpc(device_config, *request);

        const auto device_info = device_manager_->add_device(device_config);
        device_info_to_rpc(*response, device_info);
    });
}

grpc::Status DriverService::delete_device(grpc::ServerContext* context,
    const MesgDeviceSelector* request,
    MesgNone* response)
{
    return execute_command_("delete_device", [=]() {
        if (request->has_index()) {
            device_manager_->delete_device(request->index());
        } else {
            device_manager_->delete_device(request->uid());
        }
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
    catch (std::exception& e) {
        spdlog::error("command {} failed with UNKNOWN error: {}", name, e.what());
        return grpc::Status(grpc::UNKNOWN, e.what());
    }
}

} // namespace rocvad
