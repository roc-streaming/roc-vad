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

void print_driver_and_client_info(const PrDriverInfo& driver_info);

void print_device_info(const PrDeviceInfo& device_info);
void print_device_list(const PrDeviceList& device_list, bool show_info);

} // namespace rocvad
