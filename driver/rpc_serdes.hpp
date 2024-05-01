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

#include <vector>

namespace rocvad {

void device_list_from_rpc(std::vector<DeviceInfo>& out, const rvpb::RvDeviceList& in);
void device_list_to_rpc(rvpb::RvDeviceList& out, const std::vector<DeviceInfo>& in);

void device_info_from_rpc(DeviceInfo& out, const rvpb::RvDeviceInfo& in);
void device_info_to_rpc(rvpb::RvDeviceInfo& out, const DeviceInfo& in);

void packet_encoding_from_rpc(DevicePacketEncoding& out,
    const rvpb::RvPacketEncoding& in);
void packet_encoding_to_rpc(rvpb::RvPacketEncoding& out, const DevicePacketEncoding& in);

void endpoint_info_from_rpc(DeviceEndpointInfo& out, const rvpb::RvEndpointInfo& in);
void endpoint_info_to_rpc(rvpb::RvEndpointInfo& out, const DeviceEndpointInfo& in);

} // namespace rocvad
