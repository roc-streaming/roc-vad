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

class RingBuffer
{
public:
    RingBuffer(size_t n_samples);

    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;

    // Timestamps of buffer head and tail.
    uint64_t head_timestamp() const;
    uint64_t tail_timestamp() const;

    // How much samples should be appended to ring buffer so that
    // samples up to timestamp could be read.
    size_t n_need_write(uint64_t timestamp) noexcept;

    // Write samples to buffer.
    // Automatically moves buffer tail when needed.
    // Automatically moves buffer head if tail goes too far.
    void write(uint64_t timestamp, const float* samples, size_t n_samples) noexcept;

    // How much samples since timestamp were written to ring buffer
    // and can be read now.
    size_t n_can_read(uint64_t timestamp) noexcept;

    // Read samples from buffer.
    void read(uint64_t timestamp, float* samples, size_t n_samples) noexcept;

private:
    std::vector<float> mem_;
};

} // namespace rocvad
