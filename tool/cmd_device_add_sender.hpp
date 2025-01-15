/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "cmd_base.hpp"

#include <cstdint>
#include <optional>

namespace rocvad {

class CmdDeviceAddSender : public CmdBase
{
public:
    CmdDeviceAddSender(CLI::App& parent);

    bool execute(const Environment& env) override;

private:
    std::optional<std::string> device_uid_;
    std::optional<std::string> device_name_;
    std::optional<bool> device_disabled_;

    std::optional<uint32_t> device_encoding_rate_;
    std::optional<std::string> device_encoding_chans_;
    std::optional<uint32_t> device_encoding_tracks_;
    std::optional<std::string> device_encoding_buffer_;

    std::optional<std::string> packet_length_;
    std::optional<bool> packet_interleaving_;

    std::optional<uint8_t> packet_encoding_id_;
    std::optional<uint32_t> packet_encoding_rate_;
    std::optional<std::string> packet_encoding_format_;
    std::optional<std::string> packet_encoding_chans_;
    std::optional<uint32_t> packet_encoding_tracks_;

    std::optional<std::string> fec_encoding_;
    std::optional<uint32_t> fec_block_source_packets_;
    std::optional<uint32_t> fec_block_repair_packets_;

    std::optional<std::string> resampler_backend_;
    std::optional<std::string> resampler_profile_;

    std::optional<std::string> latency_tuner_backend_;
    std::optional<std::string> latency_tuner_profile_;

    std::optional<std::string> target_latency_;
    std::optional<std::string> latency_tolerance_;
};

} // namespace rocvad
