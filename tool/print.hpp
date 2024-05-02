/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "driver_protocol.hpp"

namespace rocvad {

void print_driver_and_client_info(const rvpb::RvDriverInfo& driver_info);

void print_device_info(const rvpb::RvDeviceInfo& device_info);
void print_device_list(const rvpb::RvDeviceList& device_list, bool show_info);

void print_device_encoding(const rvpb::RvDeviceEncoding& encoding);
void print_packet_encoding(const rvpb::RvPacketEncoding& encoding);

void print_endpoint_info(const rvpb::RvEndpointInfo& endpoint_info);
void print_endpoint_list(
    const google::protobuf::RepeatedPtrField<rvpb::RvEndpointInfo>& endpoint_list);

} // namespace rocvad
