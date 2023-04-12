/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "driver.hpp"

#include <CoreAudio/AudioServerPlugIn.h>

using namespace rocvad;

extern "C" __attribute__((visibility("default"))) void* entry_point(
    CFAllocatorRef allocator,
    CFUUIDRef typeUUID)
{
    (void)allocator;

    if (!CFEqual(typeUUID, kAudioServerPlugInTypeUUID)) {
        return nullptr;
    }

    static Driver driver;

    return driver.reference();
}
