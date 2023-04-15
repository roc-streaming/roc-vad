/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <aspl/Device.hpp>
#include <aspl/Plugin.hpp>

#include <memory>

#include "device_defs.hpp"
#include "index_allocator.hpp"
#include "uid_generator.hpp"

namespace rocvad {

// Correspond to one virtual device.
class Device
{
public:
    Device(std::shared_ptr<aspl::Plugin> plugin,
        IndexAllocator& index_allocator,
        UidGenerator& uid_generator,
        const DeviceConfig& config);
    ~Device();

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    DeviceInfo info();

private:
    // to generate new device index and uid
    IndexAllocator& index_allocator_;
    UidGenerator& uid_generator_;

    // objects registered in coreaudiod
    // aspl::Plugin is parent object for all devices
    // aspl::Device correspond to this specific device
    std::shared_ptr<aspl::Plugin> plugin_;
    std::shared_ptr<aspl::Device> device_;

    // run-time device info
    DeviceInfo info_;
};

} // namespace rocvad
