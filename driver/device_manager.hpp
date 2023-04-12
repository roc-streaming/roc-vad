/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "device.hpp"

#include <aspl/Plugin.hpp>

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace rocvad {

class DeviceManager
{
public:
    DeviceManager(std::shared_ptr<aspl::Plugin> plugin);

    DeviceManager(const DeviceManager&) = delete;
    DeviceManager& operator=(const DeviceManager&) = delete;

    void add_device();
    void delete_device();

private:
    std::shared_ptr<aspl::Plugin> plugin_;

    std::mutex device_mutex_;
    std::unordered_map<std::string, std::shared_ptr<Device>> devices_;
};

} // namespace rocvad
