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
#include "request_handler.hpp"
#include "transceiver.hpp"
#include "uid_generator.hpp"

#include <aspl/Device.hpp>
#include <aspl/Plugin.hpp>

#include <memory>

namespace rocvad {

// Corresponds to one virtual device.
//
// Handles requests from HAL (by implementing aspl::ControlRequestHandler and
// aspl::IORequestHandler), and provides methods to DeviceManager to control
// device state (which in turn provides methods for gRPC service).
//
// Consists of:
//  - aspl::Device   - device representative in coreaudio HAL
//  - Transceiver    - network sender or receiver (depending on device type)
//  - RequestHandler - handler for requests from HAL (uses Transceiver)
class Device
{
public:
    Device(std::shared_ptr<aspl::Plugin> hal_plugin,
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
    std::shared_ptr<aspl::Plugin> hal_plugin_;
    std::shared_ptr<aspl::Device> hal_device_;

    // network sender or receiver
    // which one is used depends on device type
    std::shared_ptr<Transceiver> net_transceiver_;

    // handler for control and I/O requests from HAL
    std::shared_ptr<RequestHandler> req_handler_;

    // run-time device info
    DeviceInfo info_;
};

} // namespace rocvad
