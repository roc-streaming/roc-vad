/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "device_storage.hpp"
#include "driver_protocol.hpp"
#include "rpc_serdes.hpp"

#include <spdlog/spdlog.h>

#include <cassert>

namespace rocvad {

DeviceStorage::DeviceStorage(std::shared_ptr<aspl::Storage> hal_storage)
    : hal_storage_(hal_storage)
{
    assert(hal_storage_);
}

std::vector<DeviceInfo> DeviceStorage::load_devices()
{
    auto [bytes, ok] = hal_storage_->ReadBytes("device_list");
    if (!ok) {
        spdlog::debug("found no device list in persistent storage");
        return {};
    }

    if (bytes.size() == 0) {
        spdlog::debug("loaded empty device list from persistent storage");
        return {};
    }

    rvpb::RvDeviceList msg;
    if (!msg.ParseFromArray(bytes.data(), bytes.size())) {
        spdlog::warn("ignoring invalid device list from persistent storage");
        return {};
    }

    spdlog::debug("loaded device list from persistent storage");

    try {
        std::vector<DeviceInfo> devices;
        device_list_from_rpc(devices, msg);

        spdlog::debug("parsed {} devices", devices.size());
        return devices;
    }
    catch (std::exception& e) {
        spdlog::warn(
            "ignoring unparseable device list from persistent storage: {}", e.what());
        return {};
    }
}

void DeviceStorage::save_devices(const std::vector<DeviceInfo>& devices)
{
    rvpb::RvDeviceList msg;
    device_list_to_rpc(msg, devices);

    std::vector<UInt8> bytes(msg.ByteSizeLong());

    if (bytes.size() != 0) {
        msg.SerializeToArray(bytes.data(), bytes.size());
    }

    if (!hal_storage_->WriteBytes("device_list", bytes)) {
        spdlog::warn("failed to save device list to persistent storage");
        return;
    }

    spdlog::debug("saved updated device list to persistent storage");
}

} // namespace rocvad
