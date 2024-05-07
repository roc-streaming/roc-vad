/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ring_buffer.hpp"

#include <algorithm>
#include <cassert>

namespace rocvad {

RingBuffer::RingBuffer(size_t n_samples)
    : buf_data_(n_samples)
    , buf_size_(n_samples)
{
    restart();
}

bool RingBuffer::first_write() const
{
    return first_write_;
}

void RingBuffer::restart()
{
    std::fill_n(&buf_data_[0], buf_size_, 0);
    buf_off_ = 0;

    head_ts_ = 0;
    tail_ts_ = 0;

    first_write_ = true;
}

RingBuffer::timestamp_t RingBuffer::head_timestamp() const
{
    return head_ts_;
}

RingBuffer::timestamp_t RingBuffer::tail_timestamp() const
{
    return tail_ts_;
}

size_t RingBuffer::n_need_write(timestamp_t timestamp) noexcept
{
    if (first_write_ || timestamp < tail_ts_) {
        return 0;
    } else {
        return std::min(size_t(timestamp - tail_ts_), buf_size_);
    }
}

void RingBuffer::write(timestamp_t timestamp,
    const float* samples,
    size_t n_samples) noexcept
{
    assert(samples);

    if (n_samples == 0) {
        return;
    }

    if (first_write_) {
        if (timestamp + n_samples < buf_size_) {
            return;
        }

        tail_ts_ = timestamp + n_samples;
        head_ts_ = tail_ts_ - buf_size_;

        buf_off_ = 0;

        first_write_ = false;
    } else {
        const timestamp_t old_head_ts = head_ts_;

        tail_ts_ = std::max(tail_ts_, timestamp + n_samples);
        head_ts_ = std::max(head_ts_, tail_ts_ - buf_size_);

        if (head_ts_ != old_head_ts) {
            buf_off_ = (buf_off_ + (head_ts_ - old_head_ts)) % buf_size_;
        }
    }

    n_samples = std::min(n_samples, buf_size_);

    copy_to_buffer_(timestamp, samples, n_samples);
}

size_t RingBuffer::n_can_read(timestamp_t timestamp) noexcept
{
    if (first_write_ || timestamp >= tail_ts_) {
        return 0;
    } else {
        return std::min(size_t(tail_ts_ - timestamp), buf_size_);
    }
}

void RingBuffer::read(timestamp_t timestamp, float* samples, size_t n_samples) noexcept
{
    assert(samples);

    if (n_samples == 0) {
        return;
    }

    if (first_write_) {
        std::fill_n(samples, n_samples, 0);
        return;
    }

    const timestamp_t from_ts = std::max(timestamp, head_ts_);
    const timestamp_t to_ts = std::min(timestamp + n_samples, tail_ts_);

    const size_t n_from_buffer = to_ts - from_ts;

    const size_t n_zeros_before = from_ts - timestamp;
    const size_t n_zeros_after = timestamp + n_samples - to_ts;

    if (n_zeros_before > 0) {
        std::fill_n(samples, n_zeros_before, 0);
    }

    if (from_ts < to_ts) {
        copy_from_buffer_(from_ts, samples + n_zeros_before, n_from_buffer);
    }

    if (n_zeros_after > 0) {
        std::fill_n(samples + n_zeros_before + n_from_buffer, n_zeros_after, 0);
    }
}

void RingBuffer::copy_to_buffer_(timestamp_t timestamp,
    const float* samples,
    size_t n_samples) noexcept
{
    assert(timestamp >= head_ts_);
    assert(timestamp + n_samples <= tail_ts_);

    const timestamp_t begin_off = (buf_off_ + timestamp - head_ts_) % buf_size_;
    const timestamp_t end_off = (begin_off + n_samples) % buf_size_;

    if (begin_off <= end_off) {
        std::copy_n(samples, end_off - begin_off, &buf_data_[begin_off]);
    } else {
        std::copy_n(samples, buf_size_ - begin_off, &buf_data_[begin_off]);
        std::copy_n(samples + buf_size_ - begin_off, end_off, &buf_data_[0]);
    }
}

void RingBuffer::copy_from_buffer_(timestamp_t timestamp,
    float* samples,
    size_t n_samples) noexcept
{
    assert(timestamp >= head_ts_);
    assert(timestamp + n_samples <= tail_ts_);

    const timestamp_t begin_off = (buf_off_ + timestamp - head_ts_) % buf_size_;
    const timestamp_t end_off = (begin_off + n_samples) % buf_size_;

    if (begin_off <= end_off) {
        std::copy_n(&buf_data_[begin_off], end_off - begin_off, samples);
    } else {
        std::copy_n(&buf_data_[begin_off], buf_size_ - begin_off, samples);
        std::copy_n(&buf_data_[0], end_off, samples + buf_size_ - begin_off);
    }
}

} // namespace rocvad
