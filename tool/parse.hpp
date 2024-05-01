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

#include <google/protobuf/util/time_util.h>
#include <spdlog/spdlog.h>

#include <cstdint>
#include <string>

namespace rocvad {

bool parse_index(const std::string& in, uint32_t& out);

std::string supported_duration_suffixes();

bool parse_duration(const char* name, const std::string& in, int64_t& out);

bool parse_duration(const char* name,
    const std::string& in,
    google::protobuf::Duration& out);

template <class Map>
std::string supported_enum_values(const Map& map)
{
    std::string supported_values;

    for (auto entry : map) {
        if (!supported_values.empty()) {
            supported_values += ", ";
        }
        supported_values += "\"";
        supported_values += std::get<2>(entry);
        supported_values += "\"";
    }

    return supported_values;
}

template <class Map, class Enum>
bool parse_enum(const char* name, const Map& map, const std::string& in, Enum& out)
{
    for (auto entry : map) {
        if (std::get<2>(entry) == in) {
            out = std::get<0>(entry);
            return true;
        }
    }

    spdlog::error("invalid {} value \"{}\", supported values: {}",
        name,
        in,
        supported_enum_values(map));

    return false;
}

} // namespace rocvad
