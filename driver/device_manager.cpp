/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "device_manager.hpp"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <cassert>
#include <stdexcept>

namespace rocvad {

namespace {

std::shared_ptr<roc_context> make_network_context()
{
    roc_context_config config;
    memset(&config, 0, sizeof(config));

    roc_context* context = nullptr;
    int err = roc_context_open(&config, &context);
    if (err < 0) {
        spdlog::critical("can't open network context: err={}", err);
        return {};
    }

    return std::shared_ptr<roc_context>(context, [](roc_context* context) {
        int err = roc_context_close(context);
        if (err < 0) {
            spdlog::critical("can't close network context: err={}", err);
        }
    });
}

} // namespace

DeviceManager::DeviceManager(std::shared_ptr<aspl::Plugin> hal_plugin,
    std::shared_ptr<aspl::Storage> hal_storage)
    : hal_plugin_(hal_plugin)
    , device_storage_(hal_storage)
{
    assert(hal_plugin_);

    network_context_ = make_network_context();

    load_devices_();
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

DeviceInfo DeviceManager::add_device(DeviceInfo info)
{
    std::lock_guard lock(mutex_);

    if (info.index != 0 && device_by_index_.count(info.index)) {
        throw std::invalid_argument(
            fmt::format("device with index {} already exists", info.index));
    }

    if (!info.uid.empty() && device_by_uid_.count(info.uid)) {
        throw std::invalid_argument(
            fmt::format("device with uid \"{}\" already exists", info.uid));
    }

    auto device = std::make_shared<Device>(
        hal_plugin_, network_context_, index_allocator_, uid_generator_, info);

    info = device->info();

    assert(info.index != 0);
    assert(!info.uid.empty());

    assert(!device_by_index_.count(info.index));
    assert(!device_by_uid_.count(info.uid));

    device_by_index_[info.index] = device;
    device_by_uid_[info.uid] = device;

    save_devices_();

    return info;
}

void DeviceManager::delete_device(index_t index)
{
    std::lock_guard lock(mutex_);

    auto device = find_device_(index);
    auto info = device->info();

    assert(device_by_index_.count(info.index));
    assert(device_by_uid_.count(info.uid));

    device_by_index_.erase(info.index);
    device_by_uid_.erase(info.uid);

    save_devices_();
}

void DeviceManager::delete_device(const std::string& uid)
{
    std::lock_guard lock(mutex_);

    auto device = find_device_(uid);
    auto info = device->info();

    assert(device_by_index_.count(info.index));
    assert(device_by_uid_.count(info.uid));

    device_by_index_.erase(info.index);
    device_by_uid_.erase(info.uid);

    save_devices_();
}

void DeviceManager::toggle_device(index_t index, bool enabled)
{
    std::lock_guard lock(mutex_);

    auto device = find_device_(index);
    device->toggle(enabled);

    save_devices_();
}

void DeviceManager::toggle_device(const std::string& uid, bool enabled)
{
    std::lock_guard lock(mutex_);

    auto device = find_device_(uid);
    device->toggle(enabled);

    save_devices_();
}

DeviceEndpointInfo DeviceManager::bind_device(index_t index, DeviceEndpointInfo endpoint)
{
    std::lock_guard lock(mutex_);

    auto device = find_device_(index);
    auto info = device->bind(endpoint);

    save_devices_();

    return info;
}

DeviceEndpointInfo DeviceManager::bind_device(const std::string& uid,
    DeviceEndpointInfo endpoint)
{
    std::lock_guard lock(mutex_);

    auto device = find_device_(uid);
    auto info = device->bind(endpoint);

    save_devices_();

    return info;
}

DeviceEndpointInfo DeviceManager::connect_device(index_t index,
    DeviceEndpointInfo endpoint)
{
    std::lock_guard lock(mutex_);

    auto device = find_device_(index);
    auto info = device->connect(endpoint);

    save_devices_();

    return info;
}

DeviceEndpointInfo DeviceManager::connect_device(const std::string& uid,
    DeviceEndpointInfo endpoint)
{
    std::lock_guard lock(mutex_);

    auto device = find_device_(uid);
    auto info = device->connect(endpoint);

    save_devices_();

    return info;
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

void DeviceManager::load_devices_()
{
    auto device_list = device_storage_.load_devices();
    if (device_list.empty()) {
        return;
    }

    spdlog::info("adding {} device(s) from persistent storage", device_list.size());

    for (const auto& device_info : device_list) {
        try {
            add_device(device_info);
        }
        catch (std::exception& e) {
            spdlog::warn("ignoring invalid device: index={}, uid=\"{}\": {}",
                device_info.index,
                device_info.uid,
                e.what());
        }
    }
}

void DeviceManager::save_devices_()
{
    auto device_list = get_all_devices();

    device_storage_.save_devices(device_list);
}

} // namespace rocvad
