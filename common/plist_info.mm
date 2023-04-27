/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "plist_info.hpp"
#include "build_info.hpp"

#include <spdlog/spdlog.h>

#import <Foundation/Foundation.h>

namespace rocvad {

namespace {

std::string read_plist(const char* bundle_id,
    const char* bundle_path,
    const char* key,
    const char* default_value,
    bool quiet)
{
    NSBundle* bundle = nullptr;

    if ((bundle = [NSBundle
             bundleWithIdentifier:[NSString stringWithUTF8String:bundle_id]])) {
        spdlog::info("found driver bundle \"{}\"", bundle_id);
    }

    if (!bundle) {
        if ((bundle = [NSBundle
                 bundleWithPath:[NSString stringWithUTF8String:bundle_path]])) {
            spdlog::info("found driver bundle at \"{}\"", bundle_path);
        }
    }

    if (!bundle) {
        spdlog::log(quiet ? spdlog::level::info : spdlog::level::warn,
            "can't find driver bundle by id \"{}\" or path \"{}\"",
            bundle_id,
            bundle_path);
        return default_value;
    }

    NSString* value =
        [bundle objectForInfoDictionaryKey:[NSString stringWithUTF8String:key]];

    if (!value) {
        spdlog::log(quiet ? spdlog::level::info : spdlog::level::warn,
            "can't find value with key \"{}\" in driver bundle",
            key);
        return default_value;
    }

    std::string ret_value = [value UTF8String];

    spdlog::debug(
        "obtained value \"{}\" for key \"{}\" from driver bundle", ret_value, key);

    return ret_value;
}

} // namespace

std::string PlistInfo::driver_socket(bool quiet)
{
    return read_plist(BuildInfo::driver_bundle_id,
        BuildInfo::driver_bundle_path,
        "DriverSocket",
        BuildInfo::driver_socket,
        quiet);
}

} // namespace rocvad
