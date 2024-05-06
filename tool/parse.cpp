/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "parse.hpp"

#include <spdlog/spdlog.h>

#include <cstdlib>
#include <limits>
#include <stdexcept>

namespace rocvad {

namespace {

const char* find_suffix(const char* str, const char* suffix)
{
    const size_t str_len = strlen(str);
    const size_t suf_len = strlen(suffix);

    if (str_len < suf_len) {
        return nullptr;
    }

    if (strcmp(str + str_len - suf_len, suffix) != 0) {
        return nullptr;
    }

    return str + str_len - suf_len;
}

} // namespace

bool parse_index(const std::string& in, uint32_t& out)
{
    const char* str = in.c_str();

    if (!isdigit(*str) && *str != '-') {
        spdlog::error("invalid index \"{}\", not a number", str);
        return false;
    }

    char* number_end = nullptr;
    long number = std::strtol(in.c_str(), &number_end, 10);

    if (number == LONG_MAX || number == LONG_MIN || !number_end || *number_end != '\0') {
        spdlog::error("invalid index \"{}\", bad number", str);
        return false;
    }

    if (number <= 0) {
        spdlog::error("invalid index \"{}\", should be positive", str);
        return false;
    }

    if (number > UINT32_MAX) {
        spdlog::error("invalid index \"{}\", out of range", str);
        return false;
    }

    out = (uint32_t)number;
    return true;
}

std::string supported_duration_suffixes()
{
    return "\"ns\", \"us\", \"ms\", \"s\", \"m\", \"h\"";
}

bool parse_duration(const char* name, const std::string& in, int64_t& result)
{
    int64_t multiplier = 0;

    const char* str = in.c_str();
    const char* suffix = nullptr;

    if ((suffix = find_suffix(str, "ns"))) {
        multiplier = 1;
    } else if ((suffix = find_suffix(str, "us"))) {
        multiplier = 1'000;
    } else if ((suffix = find_suffix(str, "ms"))) {
        multiplier = 1'000'000;
    } else if ((suffix = find_suffix(str, "s"))) {
        multiplier = 1'000'000'000;
    } else if ((suffix = find_suffix(str, "m"))) {
        multiplier = 60'000'000'000;
    } else if ((suffix = find_suffix(str, "h"))) {
        multiplier = 3600'000'000'000;
    } else {
        spdlog::error("invalid {} value \"{}\", no known suffix found ({})",
            name,
            str,
            supported_duration_suffixes());
        return false;
    }

    if (str == suffix) {
        spdlog::error("invalid {} value \"{}\", missing number", name, str);
        return false;
    }

    if (!isdigit(*str) && *str != '-') {
        spdlog::error("invalid {} value \"{}\", not a number", name, str);
        return false;
    }

    char* number_end = nullptr;
    int64_t number = std::strtol(str, &number_end, 10);

    if (number == LONG_MAX || number == LONG_MIN || !number_end || number_end != suffix) {
        spdlog::error("invalid {} value \"{}\", bad number", name, str);
        return false;
    }

    result = number * multiplier;

    if (result / multiplier != number) {
        spdlog::error("invalid {} value \"{}\", out of range", name, str);
        return false;
    }

    return true;
}

bool parse_duration(const char* name,
    const std::string& in,
    google::protobuf::Duration& out)
{
    int64_t nanoseconds = 0;
    if (!parse_duration(name, in, nanoseconds)) {
        return false;
    }

    out = google::protobuf::util::TimeUtil::NanosecondsToDuration(nanoseconds);
    return true;
}

} // namespace rocvad
