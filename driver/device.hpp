/*
 * Copyright (c) 2023 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <aspl/Device.hpp>
#include <aspl/Plugin.hpp>

#include <memory>

namespace rocvad {

class Device
{
public:
    Device(std::shared_ptr<aspl::Plugin> plugin);
    ~Device();

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

private:
    std::shared_ptr<aspl::Plugin> plugin_;
    std::shared_ptr<aspl::Device> device_;
};

} // namespace rocvad
