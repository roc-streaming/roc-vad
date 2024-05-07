/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "driver.hpp"
#include "plist_info.hpp"

#include <spdlog/spdlog.h>

namespace rocvad {

namespace {

aspl::PluginParameters make_plugin_params()
{
    aspl::PluginParameters plugin_params;

    plugin_params.Manufacturer = "roc-streaming.org";

    return plugin_params;
}

} // namespace

Driver::Driver()
{
    // create log manager first to enable logging
    log_manager_ = std::make_shared<LogManager>();

    spdlog::info("creating driver");

    // setup roc
    roc_log_set_handler(log_manager_->roc_logger(), nullptr);
    roc_log_set_level(ROC_LOG_DEBUG);

    // setup libASPL
    hal_context_ = std::make_shared<aspl::Context>(log_manager_->aspl_logger());
    hal_plugin_ = std::make_shared<aspl::Plugin>(hal_context_, make_plugin_params());
    hal_storage_ = std::make_shared<aspl::Storage>(hal_context_);
    hal_driver_ = std::make_shared<aspl::Driver>(hal_context_, hal_plugin_, hal_storage_);

    // continue initialization when persistent storage is ready
    // this will invoke OnInitialize() later
    hal_driver_->SetDriverHandler(this);
}

Driver::~Driver()
{
    spdlog::info("destroying driver");

    // teardown gRPC
    if (rpc_server_) {
        rpc_server_->Shutdown();
    }
    rpc_server_ = {};

    // teardown devices
    driver_service_ = {};
    device_manager_ = {};

    // teardown roc
    roc_log_set_handler(nullptr, nullptr);
}

AudioServerPlugInDriverRef Driver::reference()
{
    spdlog::info("received reference request");

    return hal_driver_->GetReference();
}

OSStatus Driver::OnInitialize()
{
    spdlog::info("received initialization request");

    device_manager_ = std::make_shared<DeviceManager>(hal_plugin_, hal_storage_);
    driver_service_ = std::make_unique<DriverService>(log_manager_, device_manager_);

    const std::string driver_socket = PlistInfo::driver_socket();

    spdlog::info("starting rpc server at {}", driver_socket);

    grpc::ServerBuilder rpc_builder;

    rpc_builder.AddListeningPort(driver_socket, grpc::InsecureServerCredentials());
    rpc_builder.RegisterService(driver_service_.get());

    rpc_server_ = rpc_builder.BuildAndStart();

    return kAudioHardwareNoError;
}

} // namespace rocvad
