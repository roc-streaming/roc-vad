/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "device_defs.hpp"
#include "driver_protocol.hpp"

namespace rocvad {

void device_info_from_rpc(DeviceInfo& out, const PrDeviceInfo& in);
void device_info_to_rpc(PrDeviceInfo& out, const DeviceInfo& in);

void endpoint_info_from_rpc(DeviceEndpointInfo& out, const PrEndpointInfo& in);
void endpoint_info_to_rpc(PrEndpointInfo& out, const DeviceEndpointInfo& in);

} // namespace rocvad
