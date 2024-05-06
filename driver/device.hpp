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
#include "ring_buffer.hpp"
#include "transceiver.hpp"
#include "uid_generator.hpp"

#include <roc/context.h>

#include <aspl/Device.hpp>
#include <aspl/Plugin.hpp>

#include <memory>
#include <vector>

namespace rocvad {

// Correspond to one virtual device.
class Device : private aspl::ControlRequestHandler, private aspl::IORequestHandler
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
    // aspl::ControlRequestHandler
    OSStatus OnStartIO() override;
    void OnStopIO() override;

    // aspl::IORequestHandler
    void OnReadClientInput(const std::shared_ptr<aspl::Client>& client,
        const std::shared_ptr<aspl::Stream>& stream,
        Float64 zero_timestamp,
        Float64 timestamp,
        void* bytes,
        UInt32 bytes_count) override;
    void OnWriteMixedOutput(const std::shared_ptr<aspl::Stream>& stream,
        Float64 zero_timestamp,
        Float64 timestamp,
        const void* bytes,
        UInt32 bytes_count) override;

    // endpoints
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
    std::unique_ptr<Transceiver> net_transceiver_;

    // buffers
    std::vector<float> io_buf_;
    RingBuffer ring_buf_;
    uint64_t ring_buf_pos_ = 0;

    // run-time device info
    DeviceInfo info_;
};

} // namespace rocvad
