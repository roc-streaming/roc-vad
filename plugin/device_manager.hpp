/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "device_manager_protocol.hpp"

#include <aspl/Plugin.hpp>

namespace rcp {

class DeviceManager : public proto::DeviceManagerProtocol::Service
{
public:
    DeviceManager(const std::shared_ptr<aspl::Plugin>& plugin);

    grpc::Status ping(grpc::ServerContext* context,
        const proto::None* request,
        proto::None* response) override;

    grpc::Status add_device(grpc::ServerContext* context,
        const proto::AddDeviceArgs* request,
        proto::None* response) override;

    grpc::Status remove_device(grpc::ServerContext* context,
        const proto::RemoveDeviceArgs* request,
        proto::None* response) override;

private:
    std::shared_ptr<aspl::Plugin> plugin_;
};

} // namespace rcp
