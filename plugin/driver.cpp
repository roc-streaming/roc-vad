/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "driver.hpp"

namespace rcp {

Driver::Driver()
{
    auto tracer = std::make_shared<aspl::Tracer>();
    auto context = std::make_shared<aspl::Context>(tracer);

    aspl::PluginParameters pluginParams;
    pluginParams.Manufacturer = "roc-streaming.org";

    auto plugin = std::make_shared<aspl::Plugin>(context, pluginParams);

    driver_ = std::make_shared<aspl::Driver>(context, plugin);
    deviceManager_ = std::make_unique<DeviceManager>(plugin);

    grpc::ServerBuilder builder;

    // TODO: retrieve bind address from Info.plist (RpcSocketAddress)
    builder.AddListeningPort("127.0.0.1:9712", grpc::InsecureServerCredentials());
    builder.RegisterService(deviceManager_.get());

    rpcServer_ = builder.BuildAndStart();
}

Driver::~Driver()
{
    rpcServer_->Shutdown();
}

AudioServerPlugInDriverRef Driver::reference()
{
    return driver_->GetReference();
}

} // namespace rcp
