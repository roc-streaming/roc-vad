/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace rocvad {

struct BuildInfo
{
    static const char* git_version;
    static const char* git_commit;

    static const char* driver_manufacturer;
    static const char* driver_bundle_name;
    static const char* driver_bundle_id;
    static const char* driver_bundle_path;
    static const char* driver_uuid;
    static const char* driver_socket;

    static const char* tool_name;
    static const char* tool_path;
};

} // namespace rocvad
