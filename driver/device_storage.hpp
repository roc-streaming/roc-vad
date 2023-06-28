/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "device_defs.hpp"

#include <aspl/Storage.hpp>

#include <memory>
#include <vector>

namespace rocvad {

// Persistent storage for device info.
class DeviceStorage
{
public:
    DeviceStorage(std::shared_ptr<aspl::Storage> storage);

    DeviceStorage(const DeviceStorage&) = delete;
    DeviceStorage& operator=(const DeviceStorage&) = delete;

    std::vector<DeviceInfo> load_devices();

    void save_devices(const std::vector<DeviceInfo>& devices);

private:
    std::shared_ptr<aspl::Storage> storage_;
};

} // namespace rocvad
