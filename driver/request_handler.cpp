/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "request_handler.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <cassert>

namespace rocvad {

RequestHandler::RequestHandler(const std::string& device_uid,
    const DeviceLocalEncoding& device_encoding,
    std::shared_ptr<Transceiver> net_transceiver)
    : device_uid_(device_uid)
    , chan_count_(device_encoding.channel_count)
    , net_transceiver_(net_transceiver)
    , ring_buf_(device_encoding.buffer_samples * device_encoding.channel_count)
    , io_buf_(device_encoding.buffer_samples * device_encoding.channel_count)
{
    assert(net_transceiver_);
}

// Implements aspl::ControlRequestHandler.
// Called before first I/O request when first client (app) connects.
OSStatus RequestHandler::OnStartIO()
{
    spdlog::info("starting io on device {}", device_uid_);

    // prepare sender/receiver for active I/O
    net_transceiver_->resume();

    // start from clean state
    ring_buf_.restart();

    return kAudioHardwareNoError;
}

// Implements aspl::ControlRequestHandler.
// Called after last I/O request when last client (app) disconnects.
void RequestHandler::OnStopIO()
{
    spdlog::info("stopping io on device {}", device_uid_);

    // tell sender/receiver to put connection in idle state
    net_transceiver_->pause();
}

// Implements aspl::IORequestHandler.
// Called regularly if this is receiver device (virtual mic).
// Called on real-time thread, should not block
// Should read requested number samples of samples starting from requested timestamp.
// Overlapping or sparse read requests are possible when more than one app is
// reading from the device.
void RequestHandler::OnReadClientInput(const std::shared_ptr<aspl::Client>& client,
    const std::shared_ptr<aspl::Stream>& stream,
    Float64 zero_timestamp,
    Float64 timestamp,
    void* bytes,
    UInt32 bytes_count)
{
    assert(bytes);

    timestamp_t sample_ts = (timestamp_t)timestamp * chan_count_;
    float* sample_ptr = (float*)bytes;
    size_t sample_cnt = bytes_count / sizeof(float);

    // how much samples should we append to buffer to fulfill read request?
    size_t wr_samples = ring_buf_.first_write()
                            ? sample_cnt
                            : ring_buf_.n_need_write(sample_ts + sample_cnt);
    if (wr_samples > 0) {
        if (io_buf_.size() < wr_samples) {
            io_buf_.resize(wr_samples);
        }
        // it's time to request more samples from receiver and
        // append to ring buffer
        net_transceiver_->read(io_buf_.data(), wr_samples);

        ring_buf_.write(ring_buf_.first_write() ? sample_ts : ring_buf_.tail_timestamp(),
            io_buf_.data(),
            wr_samples);
    }

    // copy from ring buffer to client
    ring_buf_.read(sample_ts, sample_ptr, sample_cnt);
}

// Implements aspl::IORequestHandler.
// Called regularly if this is sender device (virtual speakers).
// Called on real-time thread, should not block.
// Should write requested number of samples starting from requested timestamp.
// Gets samples mixed from all apps that are writing to device
// (because we set EnableMixing to true in device config).
// I'm not sure if overlapping or sparse requests are possible when mixing is
// enabled, so I try to make as little assumptions about timestamps as possible.
void RequestHandler::OnWriteMixedOutput(const std::shared_ptr<aspl::Stream>& stream,
    Float64 zero_timestamp,
    Float64 timestamp,
    const void* bytes,
    UInt32 bytes_count)
{
    assert(bytes);

    timestamp_t sample_ts = (timestamp_t)timestamp * chan_count_;
    const float* sample_ptr = (const float*)bytes;
    size_t sample_cnt = bytes_count / sizeof(float);

    if (ring_buf_.first_write()) {
        ring_buf_pos_ = sample_ts;
    }

    // copy from clients to ring buffer
    ring_buf_.write(sample_ts, sample_ptr, sample_cnt);

    // how much new samples can we read from buffer since last read?
    size_t rd_samples = ring_buf_.n_can_read(ring_buf_pos_);
    if (rd_samples > 0) {
        if (io_buf_.size() < rd_samples) {
            io_buf_.resize(rd_samples);
        }
        // it's time to read more samples from ring buffer and
        // pass to sender
        ring_buf_.read(ring_buf_pos_, io_buf_.data(), rd_samples);
        ring_buf_pos_ += rd_samples;

        net_transceiver_->write(io_buf_.data(), rd_samples);
    }
}

} // namespace rocvad
