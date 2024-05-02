/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "driver_protocol.hpp"
#include "enum_map.hpp"

#include <fmt/core.h>
#include <google/protobuf/util/time_util.h>

#include <cstdint>
#include <string>

namespace rocvad {

std::string format_device_type(rvpb::RvDeviceType device_type);

std::string format_size(uint32_t size);

std::string format_duration(int64_t nanoseconds);
std::string format_duration(google::protobuf::Duration duration);

template <class Map, class Enum>
std::string format_enum(const Map& map, Enum val)
{
    for (auto entry : map) {
        if (std::get<0>(entry) == val) {
            return std::get<2>(entry);
        }
    }

    return fmt::format("unknown ({})", (int)val);
}

} // namespace rocvad
