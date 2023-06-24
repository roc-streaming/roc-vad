/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "enum_map.hpp"

namespace rocvad {

enum_map<PrInterface, roc_interface> interface_map {
    {PR_INTERFACE_CONSOLIDATED, ROC_INTERFACE_CONSOLIDATED, "consolidated"},
    {PR_INTERFACE_AUDIO_SOURCE, ROC_INTERFACE_AUDIO_SOURCE, "audiosrc"},
    {PR_INTERFACE_AUDIO_REPAIR, ROC_INTERFACE_AUDIO_REPAIR, "audiorpr"},
    {PR_INTERFACE_AUDIO_CONTROL, ROC_INTERFACE_AUDIO_CONTROL, "audioctl"},
};

enum_map<PrChannelSet, roc_channel_set> channel_set_map {
    {PR_CHANNEL_SET_STEREO, ROC_CHANNEL_SET_STEREO, "stereo"},
};

enum_map<PrPacketEncoding, roc_packet_encoding> packet_encoding_map {
    {PR_PACKET_ENCODING_AVP_L16, ROC_PACKET_ENCODING_AVP_L16, "avp/l16"},
};

enum_map<PrFecEncoding, roc_fec_encoding> fec_encoding_map {
    {PR_FEC_ENCODING_DISABLE, ROC_FEC_ENCODING_DISABLE, "disable"},
    {PR_FEC_ENCODING_RS8M, ROC_FEC_ENCODING_RS8M, "rs8m"},
    {PR_FEC_ENCODING_LDPC_STAIRCASE, ROC_FEC_ENCODING_LDPC_STAIRCASE, "ldpc"},
};

enum_map<PrResamplerBackend, roc_resampler_backend> resampler_backend_map {
    {PR_RESAMPLER_BACKEND_BUILTIN, ROC_RESAMPLER_BACKEND_BUILTIN, "builtin"},
    {PR_RESAMPLER_BACKEND_SPEEX, ROC_RESAMPLER_BACKEND_SPEEX, "speex"},
};

enum_map<PrResamplerProfile, roc_resampler_profile> resampler_profile_map {
    {PR_RESAMPLER_PROFILE_DISABLE, ROC_RESAMPLER_PROFILE_DISABLE, "disable"},
    {PR_RESAMPLER_PROFILE_HIGH, ROC_RESAMPLER_PROFILE_HIGH, "high"},
    {PR_RESAMPLER_PROFILE_MEDIUM, ROC_RESAMPLER_PROFILE_MEDIUM, "medium"},
    {PR_RESAMPLER_PROFILE_LOW, ROC_RESAMPLER_PROFILE_LOW, "low"},
};

} // namespace rocvad
