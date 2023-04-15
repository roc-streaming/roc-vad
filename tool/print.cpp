/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "print.hpp"
#include "build_info.hpp"

#include <fmt/core.h>

namespace rocvad {

void print_driver_and_client_info(const MesgDriverInfo& driver_info)
{
    fmt::println("driver is loaded");

    fmt::println("");

    fmt::println("driver:");
    fmt::println("  version: {}", driver_info.version());
    fmt::println("  commit: {}", driver_info.commit().substr(0, 7));

    fmt::println("");

    fmt::println("client:");
    fmt::println("  version: {}", BuildInfo::version);
    fmt::println("  commit: {}", std::string(BuildInfo::commit).substr(0, 7));
}

void print_device_info(const MesgDeviceInfo& device_info)
{
    fmt::println("device #{}", device_info.index());
    fmt::println("  uid:  {}", device_info.config().uid());
    fmt::println("  name: {}", device_info.config().name());
}

void print_device_list(const MesgDeviceList& device_list, bool show_info)
{
    if (!show_info) {
        fmt::println("{:<8} {:<10} {:<22} {}", //
            "index",
            "type",
            "uid",
            "name");
    }

    bool is_first = true;

    for (const auto& device_info : device_list.devices()) {
        if (show_info) {
            if (!is_first) {
                fmt::println("");
            }
            print_device_info(device_info);
        } else {
            fmt::println("{:<8} {:<10} {:<22} {}",
                device_info.index(),
                (device_info.config().type() == MesgDeviceConfig::SENDER ? "sender"
                                                                         : "receiver"),
                device_info.config().uid(),
                device_info.config().name());
        }
        is_first = false;
    }
}

} // namespace rocvad
