/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "driver/index_allocator.hpp"

#include <gtest/gtest.h>

#include <set>

using namespace rocvad;

struct IndexAllocatorTest : testing::Test
{
    using index_t = IndexAllocator::index_t;

    IndexAllocator index_alloc;
};

TEST_F(IndexAllocatorTest, allocate_release)
{
    enum
    {
        IndexCount = 50
    };

    std::set<index_t> idx_set;

    // allocate bunch of indices
    for (unsigned iter = 0; iter < IndexCount; iter++) {
        auto idx = index_alloc.allocate_and_acquire();

        ASSERT_FALSE(idx_set.count(idx));
        idx_set.insert(idx);
    }

    ASSERT_EQ(IndexCount, idx_set.size());

    // release all
    for (auto idx : idx_set) {
        index_alloc.release(idx);
    }

    // indices should be eventually reused
    int num_iter = 0;

    for (;;) {
        num_iter++;

        auto idx = index_alloc.allocate_and_acquire();

        if (idx_set.count(idx)) {
            break;
        }

        index_alloc.release(idx);
    }

    // but not immediately
    ASSERT_GT(num_iter, 20);
}

TEST_F(IndexAllocatorTest, acquire_release)
{
    enum
    {
        IndexCount = 50
    };

    // acquire bunch of indices
    for (index_t idx = 1; idx < IndexCount; idx++) {
        index_alloc.acquire(idx);
    }

    // can't acquire again
    for (index_t idx = 1; idx < IndexCount; idx++) {
        ASSERT_THROW(index_alloc.acquire(idx), std::invalid_argument);
    }

    // release all
    for (index_t idx = 1; idx < IndexCount; idx++) {
        index_alloc.release(idx);
    }

    // can acquire again
    for (index_t idx = 1; idx < IndexCount; idx++) {
        index_alloc.acquire(idx);
    }
}

TEST_F(IndexAllocatorTest, acquire_release_allocate)
{
    enum
    {
        IndexCount = 50
    };

    std::set<index_t> idx_set;

    // acquire bunch of indices
    for (index_t idx = 1; idx < IndexCount; idx++) {
        index_alloc.acquire(idx);

        idx_set.insert(idx);
    }

    // release all
    for (index_t idx = 1; idx < IndexCount; idx++) {
        index_alloc.release(idx);
    }

    // indices should be eventually reused
    int num_iter = 0;

    for (;;) {
        num_iter++;

        auto idx = index_alloc.allocate_and_acquire();

        if (idx_set.count(idx)) {
            break;
        }

        index_alloc.release(idx);
    }

    // but not immediately
    ASSERT_GT(num_iter, 20);
}

TEST_F(IndexAllocatorTest, acquire_spare)
{
    enum
    {
        IndexCount = 50
    };

    std::set<index_t> idx_set;

    // acquire some indices sparely
    for (index_t idx = 1; idx < IndexCount; idx += 7) {
        index_alloc.acquire(idx);

        idx_set.insert(idx);
    }

    // allocate unused gaps
    for (unsigned iter = 0; iter < IndexCount; iter++) {
        auto idx = index_alloc.allocate_and_acquire();

        ASSERT_GT(idx, 0);
        ASSERT_LT(idx, IndexCount * 2);

        ASSERT_FALSE(idx_set.count(idx));
        idx_set.insert(idx);
    }
}

TEST_F(IndexAllocatorTest, release_spare)
{
    enum
    {
        IndexCount = 50
    };

    std::set<index_t> idx_set;

    // acquire bunch of indices
    for (index_t idx = 1; idx < IndexCount; idx++) {
        index_alloc.acquire(idx);

        idx_set.insert(idx);
    }

    // release most indices
    for (index_t idx = 1; idx < IndexCount; idx++) {
        if (idx % 7 != 0) {
            index_alloc.release(idx);

            idx_set.erase(idx);
        }
    }

    // allocate unused gaps
    for (unsigned iter = 0; iter < IndexCount; iter++) {
        auto idx = index_alloc.allocate_and_acquire();

        ASSERT_GT(idx, 0);
        ASSERT_LT(idx, IndexCount * 2);

        ASSERT_FALSE(idx_set.count(idx));
        idx_set.insert(idx);
    }
}
