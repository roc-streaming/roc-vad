/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "device_manager.hpp"

namespace rocvad {

DeviceManager::DeviceManager(std::shared_ptr<aspl::Plugin> plugin)
    : plugin_(plugin)
{
}

void DeviceManager::add_device()
{
    std::unique_lock lock(device_mutex_);

    devices_["TODO"] = std::make_shared<Device>(plugin_);
}

void DeviceManager::delete_device()
{
    std::unique_lock lock(device_mutex_);

    devices_.erase("TODO");
}

} // namespace rocvad
