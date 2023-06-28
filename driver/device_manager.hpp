/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "device.hpp"
#include "device_storage.hpp"
#include "index_allocator.hpp"
#include "uid_generator.hpp"

#include <aspl/Plugin.hpp>
#include <aspl/Storage.hpp>

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace rocvad {

// Contains table of devices.
class DeviceManager
{
public:
    using index_t = IndexAllocator::index_t;

    DeviceManager(std::shared_ptr<aspl::Plugin> plugin,
        std::shared_ptr<aspl::Storage> storage);

    DeviceManager(const DeviceManager&) = delete;
    DeviceManager& operator=(const DeviceManager&) = delete;

    std::vector<DeviceInfo> get_all_devices();

    DeviceInfo get_device(index_t index);
    DeviceInfo get_device(const std::string& uid);

    DeviceInfo add_device(DeviceInfo info);

    void delete_device(index_t index);
    void delete_device(const std::string& uid);

    DeviceEndpointInfo bind_device(index_t index, DeviceEndpointInfo endpoint);
    DeviceEndpointInfo bind_device(const std::string& uid, DeviceEndpointInfo endpoint);

    DeviceEndpointInfo connect_device(index_t index, DeviceEndpointInfo endpoint);
    DeviceEndpointInfo connect_device(const std::string& uid,
        DeviceEndpointInfo endpoint);

private:
    std::shared_ptr<Device> find_device_(index_t index);
    std::shared_ptr<Device> find_device_(const std::string& uid);

    void load_devices_();
    void save_devices_();

    std::recursive_mutex mutex_;

    std::shared_ptr<aspl::Plugin> plugin_;

    std::map<uint32_t, std::shared_ptr<Device>> device_by_index_;
    std::unordered_map<std::string, std::shared_ptr<Device>> device_by_uid_;

    IndexAllocator index_allocator_;
    UidGenerator uid_generator_;

    DeviceStorage device_storage_;
};

} // namespace rocvad
