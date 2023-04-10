/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "device.hpp"

#include <spdlog/spdlog.h>

namespace rcp {

Device::Device(std::shared_ptr<aspl::Plugin> plugin)
    : plugin_(plugin)
{
    spdlog::info("creating device object");

    aspl::DeviceParameters deviceParams;
    deviceParams.Name = "Roc Device";
    deviceParams.SampleRate = 44100;
    deviceParams.ChannelCount = 2;
    deviceParams.EnableMixing = true;
    deviceParams.EnableRealtimeTracing = false;

    device_ = std::make_shared<aspl::Device>(plugin->GetContext(), deviceParams);

    device_->AddStreamWithControlsAsync(aspl::Direction::Output);

    plugin_->AddDevice(device_);
}

Device::~Device()
{
    spdlog::info("destroying device object");

    plugin_->RemoveDevice(device_);
}

} // namespace rcp
