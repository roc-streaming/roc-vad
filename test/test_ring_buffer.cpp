/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "driver/ring_buffer.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <string>

using namespace rocvad;

struct RingBufferTest : testing::Test
{
    using timestamp_t = RingBuffer::timestamp_t;

    // repeat 200 times
    static constexpr size_t n_iters = 200;

    // 7 batches x 10 samples
    static constexpr size_t buf_size = 70;
    static constexpr size_t batch_size = 10;

    // writing starts with arbitrary timestamp
    static constexpr timestamp_t start_ts = 1'000'000;

    float batch[batch_size];

    RingBuffer ring_buf {buf_size};

    void SetUp() override
    {
        std::fill_n(batch, batch_size, 0);
    }

    std::string format_batch(const float* data, size_t data_size)
    {
        std::stringstream ss;
        ss << "[";
        for (size_t n = 0; n < data_size; n++) {
            ss << " " << data[n];
        }
        ss << " ]";
        return ss.str();
    }

    void expect_batch(const float* data, size_t data_size, float value)
    {
        for (size_t i = 0; i < data_size; i++) {
            if (std::abs(value - data[i]) > 1e-8) {
                std::stringstream ss;
                ss << "expected all batch elements to be:\n  " << value << "\n";
                ss << "actual elements:\n  " << format_batch(data, data_size) << "\n";
                FAIL() << ss.str();
                return;
            }
        }
    }
};

TEST_F(RingBufferTest, initial)
{
    EXPECT_TRUE(ring_buf.first_write());

    EXPECT_EQ(0, ring_buf.head_timestamp());
    EXPECT_EQ(0, ring_buf.tail_timestamp());
}

TEST_F(RingBufferTest, sequential_write_read)
{
    const size_t delay_batches = 5;

    timestamp_t ts = start_ts;

    for (size_t iter = 0; iter < n_iters; iter++) {
        // append
        std::fill_n(batch, batch_size, iter);

        ring_buf.write(ts, batch, batch_size);

        const timestamp_t tail_ts = ring_buf.tail_timestamp();
        const timestamp_t head_ts = ring_buf.head_timestamp();

        ASSERT_EQ(ts + batch_size, tail_ts);
        ASSERT_EQ(ts + batch_size - buf_size, head_ts);

        // read with fixed delay
        ring_buf.read(ts - batch_size * delay_batches, batch, batch_size);

        if (iter < delay_batches) {
            expect_batch(batch, batch_size, 0);
        } else {
            expect_batch(batch, batch_size, iter - delay_batches);
        }

        ts += batch_size;
    }
}

TEST_F(RingBufferTest, overlapping_writes)
{
    const size_t delay_batches = 5;

    timestamp_t ts = start_ts;

    for (size_t iter = 0; iter < n_iters; iter++) {
        // write, overwriting half of previous batch
        std::fill_n(batch, batch_size, iter);

        ring_buf.write(ts, batch, batch_size);

        const timestamp_t tail_ts = ring_buf.tail_timestamp();
        const timestamp_t head_ts = ring_buf.head_timestamp();

        ASSERT_EQ(ts + batch_size, tail_ts);
        ASSERT_EQ(ts + batch_size - buf_size, head_ts);

        // read with fixed delay
        ring_buf.read(ts - batch_size * delay_batches, batch, batch_size);

        if (iter < delay_batches * 2) {
            expect_batch(batch, batch_size, 0);
        } else {
            expect_batch(batch, batch_size / 2, iter - (delay_batches * 2));
            expect_batch(
                batch + batch_size / 2, batch_size / 2, iter - (delay_batches * 2) + 1);
        }

        // step by half of batch
        ts += batch_size / 2;
    }
}

TEST_F(RingBufferTest, overlapping_reads)
{
    const size_t delay_batches = 5;

    timestamp_t ts = start_ts;

    for (size_t iter = 0; iter < n_iters; iter++) {
        // append
        std::fill_n(batch, batch_size, iter);

        ring_buf.write(ts, batch, batch_size);

        const timestamp_t tail_ts = ring_buf.tail_timestamp();
        const timestamp_t head_ts = ring_buf.head_timestamp();

        ASSERT_EQ(ts + batch_size, tail_ts);
        ASSERT_EQ(ts + batch_size - buf_size, head_ts);

        // read half of batch A + half of batch B
        ring_buf.read(
            ts - batch_size * delay_batches - batch_size / 2, batch, batch_size);

        if (iter <= delay_batches) {
            expect_batch(batch, batch_size, 0);
        } else {
            expect_batch(batch, batch_size / 2, iter - delay_batches - 1);
            expect_batch(batch + batch_size / 2, batch_size / 2, iter - delay_batches);
        }

        // read whole batch B
        ring_buf.read(ts - batch_size * delay_batches, batch, batch_size);

        if (iter < delay_batches) {
            expect_batch(batch, batch_size, 0);
        } else {
            expect_batch(batch, batch_size, iter - delay_batches);
        }

        ts += batch_size;
    }
}

TEST_F(RingBufferTest, read_boundaries)
{
    const size_t batches_per_buf = buf_size / batch_size;

    timestamp_t ts = start_ts;

    for (size_t iter = 0; iter < n_iters; iter++) {
        // append
        std::fill_n(batch, batch_size, iter);

        ring_buf.write(ts, batch, batch_size);

        const timestamp_t tail_ts = ring_buf.tail_timestamp();
        const timestamp_t head_ts = ring_buf.head_timestamp();

        ASSERT_EQ(ts + batch_size, tail_ts);
        ASSERT_EQ(ts + batch_size - buf_size, head_ts);

        { // read before head
            ring_buf.read(head_ts - batch_size, batch, batch_size);

            expect_batch(batch, batch_size, 0);
        }

        { // read partially before head
            ring_buf.read(head_ts - batch_size / 2, batch, batch_size);

            if (iter < batches_per_buf) {
                expect_batch(batch, batch_size, 0);
            } else {
                expect_batch(batch, batch_size / 2, 0);
                expect_batch(
                    batch + batch_size / 2, batch_size / 2, iter - batches_per_buf + 1);
            }
        }

        { // read right after head
            ring_buf.read(head_ts, batch, batch_size);

            if (iter < batches_per_buf) {
                expect_batch(batch, batch_size, 0);
            } else {
                expect_batch(batch, batch_size, iter - batches_per_buf + 1);
            }
        }

        { // read right before tail
            ring_buf.read(tail_ts - batch_size, batch, batch_size);

            expect_batch(batch, batch_size, iter);
        }

        { // read partially after tail
            ring_buf.read(tail_ts - batch_size / 2, batch, batch_size);

            expect_batch(batch, batch_size / 2, iter);
            expect_batch(batch + batch_size / 2, batch_size / 2, 0);
        }

        { // read after tail
            ring_buf.read(tail_ts, batch, batch_size);

            expect_batch(batch, batch_size, 0);
        }

        ts += batch_size;
    }
}

TEST_F(RingBufferTest, n_need_write)
{
    timestamp_t ts = start_ts;

    for (size_t iter = 0; iter < n_iters; iter++) {
        // n_need_write
        if (iter == 0) {
            ASSERT_TRUE(ring_buf.first_write());

            ASSERT_EQ(0, ring_buf.n_need_write(ts + batch_size));
            ASSERT_EQ(0, ring_buf.n_need_write(ts + batch_size * 2));
            ASSERT_EQ(0, ring_buf.n_need_write(ts - batch_size));
            ASSERT_EQ(0, ring_buf.n_need_write(ts - batch_size * 2));
        } else {
            ASSERT_FALSE(ring_buf.first_write());

            ASSERT_EQ(batch_size * 1, ring_buf.n_need_write(ts + batch_size));
            ASSERT_EQ(batch_size * 2, ring_buf.n_need_write(ts + batch_size * 2));
            ASSERT_EQ(0, ring_buf.n_need_write(ts - batch_size));
            ASSERT_EQ(0, ring_buf.n_need_write(ts - batch_size * 2));
        }

        // append
        ring_buf.write(ts, batch, batch_size);
        ts += batch_size;
    }
}

TEST_F(RingBufferTest, n_can_read)
{
    timestamp_t wr_ts = start_ts;
    timestamp_t rd_ts = start_ts - buf_size + batch_size;

    for (size_t iter = 0; iter < n_iters; iter++) {
        // append
        ring_buf.write(wr_ts, batch, batch_size);
        wr_ts += batch_size;

        // n_can_read
        ASSERT_EQ(buf_size, ring_buf.n_can_read(rd_ts));
        ASSERT_EQ(buf_size - batch_size * 1, ring_buf.n_can_read(rd_ts + batch_size));
        ASSERT_EQ(buf_size - batch_size * 2, ring_buf.n_can_read(rd_ts + batch_size * 2));
        ASSERT_EQ(buf_size, ring_buf.n_can_read(rd_ts - batch_size));
        ASSERT_EQ(buf_size, ring_buf.n_can_read(rd_ts - batch_size * 2));

        // read
        ring_buf.read(rd_ts, batch, batch_size);
        rd_ts += batch_size;
    }
}
