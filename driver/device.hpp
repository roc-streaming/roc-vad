/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "device_defs.hpp"
#include "index_allocator.hpp"
#include "uid_generator.hpp"

#include <aspl/Device.hpp>
#include <aspl/Plugin.hpp>

#include <memory>

namespace rocvad {

// Correspond to one virtual device.
class Device
{
public:
    Device(std::shared_ptr<aspl::Plugin> plugin,
        IndexAllocator& index_allocator,
        UidGenerator& uid_generator,
        const DeviceInfo& device_info);
    ~Device();

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    DeviceInfo info();

    void toggle(bool enabled);

    DeviceEndpointInfo bind(DeviceEndpointInfo endpoint_info);
    DeviceEndpointInfo connect(DeviceEndpointInfo endpoint_info);

private:
    void bind_endpoint_(DeviceEndpointInfo& endpoint_info);
    void connect_endpoint_(DeviceEndpointInfo& endpoint_info);
    void sort_endpoints_();

    // to generate new device index and uid
    IndexAllocator& index_allocator_;
    UidGenerator& uid_generator_;

    // objects registered in coreaudiod
    // aspl::Plugin is parent object for all devices
    // aspl::Device correspond to this specific device
    std::shared_ptr<aspl::Plugin> plugin_;
    std::shared_ptr<aspl::Device> device_;
    bool device_added_ = false;

    // run-time device info
    DeviceInfo info_;
};

} // namespace rocvad
