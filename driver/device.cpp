/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "device.hpp"
#include "build_info.hpp"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <cassert>

namespace rocvad {

namespace {

aspl::DeviceParameters make_device_params(const DeviceInfo& info)
{
    aspl::DeviceParameters device_params;

    device_params.Name = info.name;
    device_params.Manufacturer = BuildInfo::driver_manufacturer;
    device_params.DeviceUID = info.uid;
    device_params.ModelUID = BuildInfo::driver_bundle_id;
    device_params.SampleRate = 44100; // TODO
    device_params.ChannelCount = 2; // TODO
    device_params.EnableMixing = true;
    device_params.EnableRealtimeTracing = false;

    return device_params;
}

} // namespace

Device::Device(std::shared_ptr<aspl::Plugin> plugin,
    IndexAllocator& index_allocator,
    UidGenerator& uid_generator,
    const DeviceInfo& info)
    : index_allocator_(index_allocator)
    , uid_generator_(uid_generator)
    , plugin_(plugin)
    , info_(info)
{
    if (info_.index == 0) {
        info_.index = index_allocator_.allocate_and_acquire();
    } else {
        index_allocator_.acquire(info_.index);
    }

    if (info_.uid.empty()) {
        info_.uid = uid_generator_.generate();
    }

    if (info_.name.empty()) {
        info_.name = fmt::format("Roc Virtual Device #{}", info_.index);
    }

    spdlog::info("creating device object, index={} uid={} type={} name=\"{}\"",
        info_.index,
        info_.uid,
        info_.type,
        info_.name);

    device_ =
        std::make_shared<aspl::Device>(plugin->GetContext(), make_device_params(info_));

    device_->AddStreamWithControlsAsync(info_.type == DeviceType::Sender
                                            ? aspl::Direction::Output
                                            : aspl::Direction::Input);

    plugin_->AddDevice(device_);
}

Device::~Device()
{
    spdlog::info("destroying device object, index={} uid={} type={} name=\"{}\"",
        info_.index,
        info_.uid,
        info_.type,
        info_.name);

    plugin_->RemoveDevice(device_);

    if (info_.index != 0) {
        index_allocator_.release(info_.index);
    }
}

DeviceInfo Device::info()
{
    return info_;
}

} // namespace rocvad
