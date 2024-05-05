/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "device_defs.hpp"

#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace rocvad {

// Base class for sender and receiver
class Transceiver
{
public:
    Transceiver() = default;
    virtual ~Transceiver() = default;

    Transceiver(const Transceiver&) = delete;
    Transceiver& operator=(const Transceiver&) = delete;

    // may modify endpoint_info
    // may throw
    virtual void bind(DeviceEndpointInfo& endpoint_info) = 0;
    virtual void connect(DeviceEndpointInfo& endpoint_info) = 0;

    // called when I/O is stopped / started
    virtual void pause() noexcept = 0;
    virtual void resume() noexcept = 0;

    // for sender
    virtual void write(uint64_t timestamp, const void* bytes, size_t n_bytes) noexcept
    {
    }

    // for receiver
    virtual void read(uint64_t timestamp, void* bytes, size_t n_bytes) noexcept
    {
        memset(bytes, 0, n_bytes);
    }
};

} // namespace rocvad
