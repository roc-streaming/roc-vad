/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ring_buffer.hpp"

#include <cassert>

namespace rocvad {

RingBuffer::RingBuffer(size_t n_samples)
    : mem_(n_samples)
{
}

uint64_t RingBuffer::head_timestamp() const
{
    // TODO
    return 0;
}

uint64_t RingBuffer::tail_timestamp() const
{
    // TODO
    return 0;
}

size_t RingBuffer::n_need_write(uint64_t timestamp) noexcept
{
    // TODO
    return 0;
}

void RingBuffer::write(uint64_t timestamp,
    const float* samples,
    size_t n_samples) noexcept
{
    assert(samples);

    // TODO
}

size_t RingBuffer::n_can_read(uint64_t timestamp) noexcept
{
    // TODO
    return 0;
}

void RingBuffer::read(uint64_t timestamp, float* samples, size_t n_samples) noexcept
{
    assert(samples);

    // TODO
}

} // namespace rocvad
