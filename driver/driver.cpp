/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "driver.hpp"
#include "plist_info.hpp"
#include "tracer.hpp"

#include <spdlog/spdlog.h>

namespace rocvad {

Driver::Driver()
{
    log_manager_ = std::make_shared<LogManager>();

    spdlog::info("received initialization request");

    aspl::PluginParameters pluginParams;
    pluginParams.Manufacturer = "roc-streaming.org";

    auto context = std::make_shared<aspl::Context>(std::make_shared<Tracer>());

    plugin_ = std::make_shared<aspl::Plugin>(context, pluginParams);
    driver_ = std::make_shared<aspl::Driver>(context, plugin_);

    device_manager_ = std::make_shared<DeviceManager>(plugin_);
    driver_service_ = std::make_unique<DriverService>(log_manager_, device_manager_);

    const std::string driver_socket = PlistInfo::driver_socket();

    spdlog::info("starting rpc server at {}", driver_socket);

    grpc::ServerBuilder rpc_builder;

    rpc_builder.AddListeningPort(driver_socket, grpc::InsecureServerCredentials());
    rpc_builder.RegisterService(driver_service_.get());

    rpc_server_ = rpc_builder.BuildAndStart();
}

Driver::~Driver()
{
    spdlog::info("received deinitialization request");

    rpc_server_->Shutdown();
}

AudioServerPlugInDriverRef Driver::reference()
{
    spdlog::info("received reference request");

    return driver_->GetReference();
}

} // namespace rocvad
