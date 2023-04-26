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

// Allocates device indicies.
// Keeps indicies small and unique.
// Postpones reusing old index for a while.
// Ported from libASPL dispatcher.
class IndexAllocator
{
public:
    using index_t = uint32_t;

    IndexAllocator();

    IndexAllocator(const IndexAllocator&) = delete;
    IndexAllocator& operator=(const IndexAllocator&) = delete;

    index_t allocate_and_acquire();
    void acquire(index_t index);
    void release(index_t index);

private:
    index_t find_free_(index_t start_index) const;
    bool is_free_(index_t index) const;
    void set_free_(index_t index, bool free);

    using bitset_chunk_t = uint64_t;
    static constexpr size_t bits_per_chunk = sizeof(bitset_chunk_t) * 8;

    std::vector<bitset_chunk_t> index_bitset_;

    size_t num_allocated_ = 0;
    index_t last_allocated_index_ = 0;

    // The maximum value below which we're currently trying to keep indicies.
    // If the next index exceeds this value, and there are quite a lot of
    // free indicies below the maximum, we'll reuse one of them. Otherwise,
    // if there are too little free indicies below the maximum, we'll increase
    // the maximum instead.
    index_t desired_maximum_index_ = 99;
};

} // namespace rocvad
