/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "enum_map.hpp"

namespace rocvad {

enum_map<rvpb::RvInterface, roc_interface> interface_map {
    {rvpb::RV_INTERFACE_CONSOLIDATED, ROC_INTERFACE_CONSOLIDATED, "consolidated"},
    {rvpb::RV_INTERFACE_AUDIO_SOURCE, ROC_INTERFACE_AUDIO_SOURCE, "audiosrc"},
    {rvpb::RV_INTERFACE_AUDIO_REPAIR, ROC_INTERFACE_AUDIO_REPAIR, "audiorpr"},
    {rvpb::RV_INTERFACE_AUDIO_CONTROL, ROC_INTERFACE_AUDIO_CONTROL, "audioctl"},
};

enum_map<rvpb::RvSampleFormat, roc_format> sample_format_map {
    // FIXME
    {rvpb::ROC_FORMAT_PCM_SINT16, ROC_FORMAT_PCM_FLOAT32, "s16"},
};

enum_map<rvpb::RvChannelLayout, roc_channel_layout> channel_layout_map {
    {rvpb::RV_CHANNEL_LAYOUT_MONO, ROC_CHANNEL_LAYOUT_MONO, "mono"},
    {rvpb::RV_CHANNEL_LAYOUT_STEREO, ROC_CHANNEL_LAYOUT_STEREO, "stereo"},
    {rvpb::RV_CHANNEL_LAYOUT_MULTITRACK, ROC_CHANNEL_LAYOUT_MULTITRACK, "multitrack"},
};

enum_map<rvpb::RvFecEncoding, roc_fec_encoding> fec_encoding_map {
    {rvpb::RV_FEC_ENCODING_DISABLE, ROC_FEC_ENCODING_DISABLE, "disable"},
    {rvpb::RV_FEC_ENCODING_DEFAULT, ROC_FEC_ENCODING_DEFAULT, "default"},
    {rvpb::RV_FEC_ENCODING_RS8M, ROC_FEC_ENCODING_RS8M, "rs8m"},
    {rvpb::RV_FEC_ENCODING_LDPC_STAIRCASE, ROC_FEC_ENCODING_LDPC_STAIRCASE, "ldpc"},
};

enum_map<rvpb::RvLatencyTunerBackend, roc_latency_tuner_backend>
    latency_tuner_backend_map {
        {rvpb::RV_LATENCY_TUNER_BACKEND_DEFAULT,
            ROC_LATENCY_TUNER_BACKEND_DEFAULT,
            "default"},
        {rvpb::RV_LATENCY_TUNER_BACKEND_NIQ, ROC_LATENCY_TUNER_BACKEND_NIQ, "niq"},
    };

enum_map<rvpb::RvLatencyTunerProfile, roc_latency_tuner_profile>
    latency_tuner_profile_map {
        {rvpb::RV_LATENCY_TUNER_PROFILE_DEFAULT,
            ROC_LATENCY_TUNER_PROFILE_DEFAULT,
            "default"},
        {rvpb::RV_LATENCY_TUNER_PROFILE_INTACT,
            ROC_LATENCY_TUNER_PROFILE_INTACT,
            "intact"},
        {rvpb::RV_LATENCY_TUNER_PROFILE_RESPONSIVE,
            ROC_LATENCY_TUNER_PROFILE_RESPONSIVE,
            "responsive"},
        {rvpb::RV_LATENCY_TUNER_PROFILE_GRADUAL,
            ROC_LATENCY_TUNER_PROFILE_GRADUAL,
            "gradual"},
    };

enum_map<rvpb::RvResamplerBackend, roc_resampler_backend> resampler_backend_map {
    {rvpb::RV_RESAMPLER_BACKEND_DEFAULT, ROC_RESAMPLER_BACKEND_DEFAULT, "default"},
    {rvpb::RV_RESAMPLER_BACKEND_BUILTIN, ROC_RESAMPLER_BACKEND_BUILTIN, "builtin"},
    {rvpb::RV_RESAMPLER_BACKEND_SPEEX, ROC_RESAMPLER_BACKEND_SPEEX, "speex"},
    {rvpb::RV_RESAMPLER_BACKEND_SPEEXDEC, ROC_RESAMPLER_BACKEND_SPEEXDEC, "speexdec"},
};

enum_map<rvpb::RvResamplerProfile, roc_resampler_profile> resampler_profile_map {
    {rvpb::RV_RESAMPLER_PROFILE_DEFAULT, ROC_RESAMPLER_PROFILE_DEFAULT, "default"},
    {rvpb::RV_RESAMPLER_PROFILE_HIGH, ROC_RESAMPLER_PROFILE_HIGH, "high"},
    {rvpb::RV_RESAMPLER_PROFILE_MEDIUM, ROC_RESAMPLER_PROFILE_MEDIUM, "medium"},
    {rvpb::RV_RESAMPLER_PROFILE_LOW, ROC_RESAMPLER_PROFILE_LOW, "low"},
};

} // namespace rocvad
