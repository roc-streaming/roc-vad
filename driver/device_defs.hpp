/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <fmt/core.h>

#include <string>
#include <string_view>

#include "index_allocator.hpp"

namespace rocvad {

// Device type.
enum class DeviceType
{
    Sender,
    Receiver,
};

// Device creation parameters.
struct DeviceConfig
{
    DeviceType type = DeviceType::Sender;
    std::string name;
    std::string uid;
};

// Device run-time information
struct DeviceInfo
{
    IndexAllocator::index_t index = 0;

    DeviceConfig config;

    DeviceInfo() = default;

    DeviceInfo(const DeviceConfig& config)
        : config(config)
    {
    }
};

} // namespace rocvad

//! Device type formatter.
template <>
struct fmt::formatter<rocvad::DeviceType> : fmt::formatter<std::string_view>
{
    auto format(rocvad::DeviceType device_type, fmt::format_context& ctx) const
    {
        return fmt::formatter<std::string_view>::format(
            device_type == rocvad::DeviceType::Sender ? "sender" : "receiver", ctx);
    }
};