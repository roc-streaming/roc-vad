/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <cstdint>
#include <cstdlib>
#include <vector>

namespace rocvad {

// Holds last N samples retrieved from receiver or written to sender.
// Used when HAL re-requests us same samples multiple times.
// Does not affect latency.
class RingBuffer
{
public:
    using timestamp_t = uint64_t;

    RingBuffer(size_t n_samples);

    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;

    // true if nothing was ever written to buffer yet
    bool first_write() const;

    // absolute timestamps of buffer head and tail
    timestamp_t head_timestamp() const;
    timestamp_t tail_timestamp() const;

    // how much samples should be appended to ring buffer so that
    // samples up to timestamp could be read
    size_t n_need_write(timestamp_t timestamp) noexcept;

    // write samples to buffer,
    // automatically move buffer head and tail when needed
    void write(timestamp_t timestamp, const float* samples, size_t n_samples) noexcept;

    // how much samples since timestamp were written to ring buffer
    // and can be read now
    size_t n_can_read(timestamp_t timestamp) noexcept;

    // read samples from buffer
    void read(timestamp_t timestamp, float* samples, size_t n_samples) noexcept;

private:
    void copy_to_buffer_(timestamp_t timestamp,
        const float* samples,
        size_t num_samples) noexcept;
    void copy_from_buffer_(timestamp_t timestamp,
        float* samples,
        size_t num_samples) noexcept;

    std::vector<float> buf_data_;
    const size_t buf_size_;
    // offset of logical head in physical buffer
    size_t buf_off_ = 0;

    // absolute timestamps of logical head and tail
    timestamp_t head_ts_ = 0;
    timestamp_t tail_ts_ = 0;

    bool first_write_ = true;
};

} // namespace rocvad
