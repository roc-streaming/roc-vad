/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "receiver.hpp"

namespace rocvad {

Receiver::Receiver(std::shared_ptr<roc_context> network_context,
    const std::string& uid,
    const DeviceLocalEncoding& device_encoding,
    const DeviceReceiverConfig& receiver_config)
{
}

void Receiver::bind(DeviceEndpointInfo& endpoint_info)
{
}

void Receiver::connect(DeviceEndpointInfo& endpoint_info)
{
}

void Receiver::pause() noexcept
{
}

void Receiver::resume() noexcept
{
}

void Receiver::read(uint64_t timestamp, void* bytes, size_t n_bytes) noexcept
{
}

} // namespace rocvad
