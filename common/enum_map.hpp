/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "driver_protocol.hpp"

#include <roc/config.h>

#include <string>
#include <tuple>
#include <vector>

namespace rocvad {

template <class E1, class E2>
using enum_map = std::vector<std::tuple<E1, E2, std::string>>;

extern enum_map<PrChannelSet, roc_channel_set> channel_set_map;
extern enum_map<PrPacketEncoding, roc_packet_encoding> packet_encoding_map;
extern enum_map<PrFecEncoding, roc_fec_encoding> fec_encoding_map;
extern enum_map<PrResamplerBackend, roc_resampler_backend> resampler_backend_map;
extern enum_map<PrResamplerProfile, roc_resampler_profile> resampler_profile_map;

} // namespace rocvad
