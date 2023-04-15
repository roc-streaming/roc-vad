/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "index_allocator.hpp"

#include <spdlog/spdlog.h>

#include <cassert>
#include <limits>

namespace rocvad {

IndexAllocator::IndexAllocator()
{
    // never use zero index
    set_free_(0, false);
    last_allocated_index_ = 0;
    num_allocated_ = 1;
}

IndexAllocator::index_t IndexAllocator::allocate()
{
    index_t next_index = last_allocated_index_ + 1;

    if (next_index >= desired_maximum_index_ &&
        num_allocated_ > desired_maximum_index_ / 2) {
        if (desired_maximum_index_ <= std::numeric_limits<index_t>::max() / 2) {
            spdlog::trace("increasing desired maximum from {} to {}",
                desired_maximum_index_,
                desired_maximum_index_ * 2);
            desired_maximum_index_ *= 2;
        }
    }

    const bool is_free = is_free_(next_index);

    spdlog::trace("candidate next_index={} is_free={} desired_maximum_index={}",
        next_index,
        is_free,
        desired_maximum_index_);

    if (next_index >= desired_maximum_index_) {
        next_index = find_free_(0);
    } else if (!is_free) {
        next_index = find_free_(next_index + 1);
    }

    set_free_(next_index, false);
    num_allocated_++;
    last_allocated_index_ = next_index;

    spdlog::trace("allocated index={} num_allocated={}", next_index, num_allocated_);

    return next_index;
}

void IndexAllocator::release(index_t index)
{
    set_free_(index, true);
    num_allocated_--;

    spdlog::trace("freed index={} num_allocated={}", index, num_allocated_);
}

IndexAllocator::index_t IndexAllocator::find_free_(index_t start_index) const
{
    if (index_bitset_.size() != 0) {
        const size_t start_chunk = start_index / bits_per_chunk;

        for (size_t n = 0; n <= index_bitset_.size(); n++) {
            const size_t chunk = (start_chunk + n) % index_bitset_.size();

            const auto chunk_bits = index_bitset_[chunk];

            if (chunk_bits == bitset_chunk_t(-1)) {
                continue;
            }

            for (size_t bit = 0; bit < bits_per_chunk; bit++) {
                if ((chunk_bits & (bitset_chunk_t(1) << bit)) == 0) {
                    const auto index = index_t(chunk * bits_per_chunk + bit);

                    if (n == 0 && index < start_index) {
                        continue;
                    }

                    spdlog::trace(
                        "selected id inside bitset start_index={} selected_index={}",
                        start_index,
                        index);

                    return index;
                }
            }
        }
    }

    auto index = index_t(index_bitset_.size() * bits_per_chunk);

    spdlog::trace(
        "selected id after bitset start_index=%u selected_index={}", start_index, index);

    return index;
}

bool IndexAllocator::is_free_(index_t index) const
{
    const size_t chunk = index / bits_per_chunk;
    const size_t bit = index % bits_per_chunk;

    if (index_bitset_.size() <= chunk) {
        return true;
    }

    return (index_bitset_[chunk] & (bitset_chunk_t(1) << bit)) == 0;
}

void IndexAllocator::set_free_(index_t index, bool free)
{
    const size_t chunk = index / bits_per_chunk;
    const size_t bit = index % bits_per_chunk;

    if (index_bitset_.size() <= chunk) {
        index_bitset_.resize(chunk + 1);
    }

    if (free) {
        index_bitset_[chunk] &= ~(bitset_chunk_t(1) << bit);
    } else {
        index_bitset_[chunk] |= (bitset_chunk_t(1) << bit);
    }
}

} // namespace rocvad
