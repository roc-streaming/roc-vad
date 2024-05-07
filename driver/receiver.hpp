/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "transceiver.hpp"

#include <roc/context.h>
#include <roc/receiver.h>

namespace rocvad {

class Receiver : public Transceiver
{
public:
    Receiver(const std::string& device_uid,
        const DeviceLocalEncoding& device_encoding,
        const DeviceReceiverConfig& device_receiver_config);
    ~Receiver();

    void bind(DeviceEndpointInfo& endpoint_info) override;
    void connect(DeviceEndpointInfo& endpoint_info) override;

    void pause() noexcept override;
    void resume() noexcept override;

    void read(float* samples, size_t n_samples) noexcept override;

private:
    std::string device_uid_;

    roc_context* net_context_ = nullptr;
    roc_receiver* net_receiver_ = nullptr;

    static constexpr uint64_t err_report_freq_ = 5000;
    uint64_t err_count_ = 0;
};

} // namespace rocvad
