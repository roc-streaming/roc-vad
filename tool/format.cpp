/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "format.hpp"

namespace rocvad {

std::string format_device_type(rvpb::RvDeviceType device_type)
{
    return device_type == rvpb::RV_DEVICE_TYPE_SENDER ? "sender" : "receiver";
}

std::string format_size(uint32_t size)
{
    if (size == 0) {
        return "default";
    }

    return std::to_string(size);
}

std::string format_duration(int64_t nanoseconds)
{
    if (nanoseconds == 0) {
        return "default";
    }

    int64_t multiplier = 1;
    const char* suffix = "ns";

    for (auto [m, s] : std::array<std::tuple<int64_t, const char*>, 6> {{
             {1, "ns"},
             {1'000, "us"},
             {1'000'000, "ms"},
             {1'000'000'000, "s"},
             {60'000'000'000, "m"},
             {3600'000'000'000, "h"},
         }}) {
        if (std::abs(nanoseconds) >= m) {
            multiplier = m;
            suffix = s;
        }
    }

    return fmt::format("{}{}", (double)nanoseconds / multiplier, suffix);
}

std::string format_duration(google::protobuf::Duration duration)
{
    const int64_t nanoseconds =
        google::protobuf::util::TimeUtil::DurationToNanoseconds(duration);

    return format_duration(nanoseconds);
}

} // namespace rocvad
