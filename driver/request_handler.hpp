/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "ring_buffer.hpp"
#include "transceiver.hpp"

#include <aspl/Device.hpp>

#include <memory>
#include <vector>

namespace rocvad {

// Handler for control and I/O requests to virtual device.
class RequestHandler : public aspl::ControlRequestHandler, public aspl::IORequestHandler
{
public:
    RequestHandler(const std::string& device_uid,
        const DeviceLocalEncoding& device_encoding,
        std::shared_ptr<Transceiver> net_transceiver);

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

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

private:
    using timestamp_t = RingBuffer::timestamp_t;

    const std::string device_uid_;

    const size_t chan_count_;

    std::shared_ptr<Transceiver> net_transceiver_;

    RingBuffer ring_buf_;
    timestamp_t ring_buf_pos_ = 0;

    std::vector<float> io_buf_;
};

} // namespace rocvad
