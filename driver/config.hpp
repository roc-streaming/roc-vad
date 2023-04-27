/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <roc/config.h>

#include <cstdint>
#include <optional>

namespace rocvad {

// Local parameters of device.
struct LocalConfig
{
    uint32_t sample_rate = 44100;
    roc_channel_set channel_set = ROC_CHANNEL_SET_STEREO;
};

// Network parameters of sender device.
struct SenderConfig
{
    roc_packet_encoding packet_encoding = ROC_PACKET_ENCODING_AVP_L16;
    uint64_t packet_length_ns = 7'000'000; // 7ms

    roc_fec_encoding fec_encoding = ROC_FEC_ENCODING_RS8M;
    uint32_t fec_block_source_packets = 20;
    uint32_t fec_block_repair_packets = 10;
};

// Network parameters of sender device.
struct ReceiverConfig
{
    uint64_t target_latency_ns = 100'000'000; // 100ms

    roc_resampler_backend resampler_backend = ROC_RESAMPLER_BACKEND_SPEEX;
    roc_resampler_profile resampler_profile = ROC_RESAMPLER_PROFILE_MEDIUM;
};

} // namespace rocvad
