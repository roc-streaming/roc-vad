/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <string>

namespace rocvad {

// Reads configuration from Info.plist in driver bundle.
struct PlistInfo
{
    static std::string driver_socket();
};

} // namespace rocvad
