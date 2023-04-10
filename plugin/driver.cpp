/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "driver.hpp"

#include <spdlog/spdlog.h>

namespace rcp {

Driver::Driver()
{
    log_manager_ = std::make_shared<LogManager>();

    spdlog::info("received initialization request");

    aspl::PluginParameters pluginParams;
    pluginParams.Manufacturer = "roc-streaming.org";

    auto tracer = std::make_shared<aspl::Tracer>();
    auto context = std::make_shared<aspl::Context>(tracer);

    plugin_ = std::make_shared<aspl::Plugin>(context, pluginParams);
    driver_ = std::make_shared<aspl::Driver>(context, plugin_);

    device_manager_ = std::make_shared<DeviceManager>(log_manager_, plugin_);

    grpc::ServerBuilder rpc_builder;

    // TODO: retrieve bind address from Info.plist (RpcSocketAddress)
    const auto address = "127.0.0.1:9712";

    rpc_builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    rpc_builder.RegisterService(device_manager_.get());

    spdlog::info("starting rpc server at {}", address);

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

} // namespace rcp
