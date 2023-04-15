/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "device.hpp"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

namespace rocvad {

namespace {

void populate_default_values(DeviceInfo& info)
{
    if (info.config.name.empty()) {
        info.config.name = fmt::format("Roc Virtual Device #{}", info.index);
    }
}

aspl::DeviceParameters make_device_params(const DeviceConfig& config)
{
    aspl::DeviceParameters device_params;

    device_params.Name = config.name;
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
    const DeviceConfig& config)
    : index_allocator_(index_allocator)
    , uid_generator_(uid_generator)
    , plugin_(plugin)
    , info_(config)
{
    info_.index = index_allocator_.allocate();

    if (info_.config.uid.empty()) {
        info_.config.uid = uid_generator_.generate();
    }

    populate_default_values(info_);

    spdlog::info("creating device object, index={} uid={} type={} name=\"{}\"",
        info_.index,
        info_.config.uid,
        info_.config.type,
        info_.config.name);

    device_ = std::make_shared<aspl::Device>(
        plugin->GetContext(), make_device_params(info_.config));

    device_->AddStreamWithControlsAsync(info_.config.type == DeviceType::Sender
                                            ? aspl::Direction::Output
                                            : aspl::Direction::Input);

    plugin_->AddDevice(device_);
}

Device::~Device()
{
    spdlog::info("destroying device object, index={} uid={} type={} name=\"{}\"",
        info_.index,
        info_.config.uid,
        info_.config.type,
        info_.config.name);

    plugin_->RemoveDevice(device_);

    index_allocator_.release(info_.index);
}

DeviceInfo Device::info()
{
    return info_;
}

} // namespace rocvad
