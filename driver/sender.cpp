/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "sender.hpp"

namespace rocvad {

Sender::Sender(std::shared_ptr<roc_context> network_context,
    const std::string& uid,
    const DeviceLocalEncoding& device_encoding,
    const DeviceSenderConfig& sender_config)
{
}

void Sender::bind(DeviceEndpointInfo& endpoint_info)
{
}

void Sender::connect(DeviceEndpointInfo& endpoint_info)
{
}

void Sender::pause() noexcept
{
}

void Sender::resume() noexcept
{
}

void Sender::write(uint64_t timestamp, const void* bytes, size_t n_bytes) noexcept
{
}

} // namespace rocvad
