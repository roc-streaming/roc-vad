/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <cstdint>

namespace rocvad {

// Allowed range for channel_count/tracks in case of multi-track.
static constexpr uint32_t MIN_TRACK_COUNT = 1;
static constexpr uint32_t MAX_TRACK_COUNT = 1024;

} // namespace rocvad
