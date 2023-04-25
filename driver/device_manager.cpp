/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "device_manager.hpp"

#include <fmt/core.h>

#include <cassert>
#include <stdexcept>

namespace rocvad {

DeviceManager::DeviceManager(std::shared_ptr<aspl::Plugin> plugin)
    : plugin_(plugin)
{
    assert(plugin_);
}

std::vector<DeviceInfo> DeviceManager::get_all_devices()
{
    std::lock_guard lock(mutex_);

    std::vector<DeviceInfo> info_list;
    info_list.reserve(device_by_index_.size());

    // sorted by index
    for (const auto& [_, device] : device_by_index_) {
        info_list.push_back(device->info());
    }

    return info_list;
}

DeviceInfo DeviceManager::get_device(index_t index)
{
    std::lock_guard lock(mutex_);

    auto device = find_device_(index);
    return device->info();
}

DeviceInfo DeviceManager::get_device(const std::string& uid)
{
    std::lock_guard lock(mutex_);

    auto device = find_device_(uid);
    return device->info();
}

DeviceInfo DeviceManager::add_device(const DeviceConfig& config)
{
    std::lock_guard lock(mutex_);

    if (!config.uid.empty() && device_by_uid_.count(config.uid)) {
        throw std::invalid_argument(
            fmt::format("device with uid \"{}\" already exists", config.uid));
    }

    auto device =
        std::make_shared<Device>(plugin_, index_allocator_, uid_generator_, config);

    auto info = device->info();

    assert(!device_by_index_.count(info.index));
    assert(!device_by_uid_.count(info.config.uid));

    device_by_index_[info.index] = device;
    device_by_uid_[info.config.uid] = device;

    return info;
}

void DeviceManager::delete_device(index_t index)
{
    std::lock_guard lock(mutex_);

    auto device = find_device_(index);
    auto info = device->info();

    assert(device_by_index_.count(info.index));
    assert(device_by_uid_.count(info.config.uid));

    device_by_index_.erase(info.index);
    device_by_uid_.erase(info.config.uid);
}

void DeviceManager::delete_device(const std::string& uid)
{
    std::lock_guard lock(mutex_);

    auto device = find_device_(uid);
    auto info = device->info();

    assert(device_by_index_.count(info.index));
    assert(device_by_uid_.count(info.config.uid));

    device_by_index_.erase(info.index);
    device_by_uid_.erase(info.config.uid);
}

std::shared_ptr<Device> DeviceManager::find_device_(index_t index)
{
    if (!device_by_index_.count(index)) {
        throw std::invalid_argument(fmt::format("device with index {} not found", index));
    }

    auto device = device_by_index_[index];
    assert(device);

    return device;
}

std::shared_ptr<Device> DeviceManager::find_device_(const std::string& uid)
{
    if (!device_by_uid_.count(uid)) {
        throw std::invalid_argument(fmt::format("device with uid \"{}\" not found", uid));
    }

    auto device = device_by_uid_[uid];
    assert(device);

    return device;
}

} // namespace rocvad
