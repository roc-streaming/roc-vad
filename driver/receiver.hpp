/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "transceiver.hpp"

#include <roc/context.h>
#include <roc/receiver.h>

namespace rocvad {

class Receiver : public Transceiver
{
public:
    Receiver(std::shared_ptr<roc_context> network_context,
        const std::string& uid,
        const DeviceLocalEncoding& device_encoding,
        const DeviceReceiverConfig& receiver_config);

    void bind(DeviceEndpointInfo& endpoint_info) override;
    void connect(DeviceEndpointInfo& endpoint_info) override;

    void pause() noexcept override;
    void resume() noexcept override;

    void read(uint64_t timestamp, void* bytes, size_t n_bytes) noexcept override;

private:
    std::string uid_;

    std::shared_ptr<roc_context> net_context_;
    std::shared_ptr<roc_receiver> net_receiver_;
};

} // namespace rocvad
