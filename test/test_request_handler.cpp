/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "driver/receiver.hpp"
#include "driver/request_handler.hpp"
#include "driver/sender.hpp"

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <future>
#include <sstream>
#include <thread>

using namespace rocvad;

// Integration test that combines:
//  - RequestHandler (sender-side)
//  - Sender (roc_sender)
//  - Receiver (roc_receiver)
//  - RequestHandler (receiver-side)
//
// Flow:
//  - thread 1 writes samples to sender-side RequestHandler
//    (i.e. it pretends to be HAL for output device)
//  - sender-side RequestHandler writes samples to roc_sender
//  - roc_sender sends samples to roc_receiver via localhost
//  - receiver-side RequestHandler reads samples from roc_receiver
//  - thread 2 reads samples from receiver-side RequestHandler
//    (i.e. it pretends to be HAL for input device)
struct RequestHandlerTest : testing::Test
{
    static constexpr size_t sample_rate = 44100;
    static constexpr size_t chan_count = 2;

    DeviceInfo sender_info {
        .type = DeviceType::Sender,
        .index = 1,
        .uid = "test_sender",
        .name = "test_sender",
        .enabled = true,
        .device_encoding =
            {
                .sample_rate = sample_rate,
                .channel_layout = ROC_CHANNEL_LAYOUT_STEREO,
                .channel_count = chan_count,
                .buffer_length_ns = 1'000'000'000,
                .buffer_samples = sample_rate * chan_count,
            },
        .sender_config =
            DeviceSenderConfig {
                .latency_tuner_profile = ROC_LATENCY_TUNER_PROFILE_INTACT,
            },
        .remote_endpoints =
            {
                DeviceEndpointInfo {
                    .slot = ROC_SLOT_DEFAULT,
                    .interface = ROC_INTERFACE_AUDIO_SOURCE,
                    .uri = "rtp+rs8m://127.0.0.1:9701",
                },
                DeviceEndpointInfo {
                    .slot = ROC_SLOT_DEFAULT,
                    .interface = ROC_INTERFACE_AUDIO_REPAIR,
                    .uri = "rs8m://127.0.0.1:9702",
                },
                DeviceEndpointInfo {
                    .slot = ROC_SLOT_DEFAULT,
                    .interface = ROC_INTERFACE_AUDIO_CONTROL,
                    .uri = "rtcp://127.0.0.1:9703",
                },
            },
    };

    DeviceInfo receiver_info {
        .type = DeviceType::Receiver,
        .index = 2,
        .uid = "test_receiver",
        .name = "test_receiver",
        .enabled = true,
        .device_encoding =
            {
                .sample_rate = sample_rate,
                .channel_layout = ROC_CHANNEL_LAYOUT_STEREO,
                .channel_count = chan_count,
                .buffer_length_ns = 1'000'000'000,
                .buffer_samples = sample_rate * chan_count,
            },
        .receiver_config =
            DeviceReceiverConfig {
                .latency_tuner_profile = ROC_LATENCY_TUNER_PROFILE_INTACT,
                .target_latency_ns = 100'000'000,
            },
        .local_endpoints =
            {
                DeviceEndpointInfo {
                    .slot = ROC_SLOT_DEFAULT,
                    .interface = ROC_INTERFACE_AUDIO_SOURCE,
                    .uri = "rtp+rs8m://127.0.0.1:9701",
                },
                DeviceEndpointInfo {
                    .slot = ROC_SLOT_DEFAULT,
                    .interface = ROC_INTERFACE_AUDIO_REPAIR,
                    .uri = "rs8m://127.0.0.1:9702",
                },
                DeviceEndpointInfo {
                    .slot = ROC_SLOT_DEFAULT,
                    .interface = ROC_INTERFACE_AUDIO_CONTROL,
                    .uri = "rtcp://127.0.0.1:9703",
                },
            },
    };

    std::shared_ptr<Sender> sender;
    std::shared_ptr<RequestHandler> sender_handler;

    std::shared_ptr<Receiver> receiver;
    std::shared_ptr<RequestHandler> receiver_handler;

    void sleep_for_n_samples(size_t n_samples)
    {
        std::this_thread::sleep_for(
            std::chrono::duration<double, std::milli>(1000.0 / sample_rate * n_samples));
    }

    void SetUp() override
    {
        receiver = std::make_shared<Receiver>(receiver_info.uid,
            receiver_info.device_encoding,
            *receiver_info.receiver_config);

        receiver_handler = std::make_shared<RequestHandler>(
            receiver_info.uid, receiver_info.device_encoding, receiver);

        for (auto& endpoint : receiver_info.local_endpoints) {
            receiver->bind(endpoint);
        }

        sender = std::make_shared<Sender>(
            sender_info.uid, sender_info.device_encoding, *sender_info.sender_config);

        sender_handler = std::make_shared<RequestHandler>(
            sender_info.uid, sender_info.device_encoding, sender);

        for (auto& endpoint : sender_info.remote_endpoints) {
            sender->connect(endpoint);
        }
    }
};

TEST_F(RequestHandlerTest, simple_loopback)
{
    static constexpr float sample_value = 0.5;

    static constexpr float wait_n_subsequent_samples =
        sample_rate * chan_count / 2; // 500ms

    std::atomic_bool sender_stop = false;

    auto sender_thread = std::async(std::launch::async, [&]() {
        sender_handler->OnStartIO();

        double ts = 1'000'000;

        while (!sender_stop) {
            float samples[100] = {};
            std::fill_n(samples, std::size(samples), sample_value);

            sender_handler->OnWriteMixedOutput(nullptr, 0, ts, samples, sizeof(samples));

            ts += std::size(samples) / chan_count;
            sleep_for_n_samples(std::size(samples) / chan_count);
        }

        sender_handler->OnStopIO();
    });

    {
        receiver_handler->OnStartIO();

        double ts = 100'000'000;
        size_t n_ok = 0;

        while (n_ok < wait_n_subsequent_samples) {
            float samples[30] = {};

            receiver_handler->OnReadClientInput(
                nullptr, nullptr, 0, ts, samples, sizeof(samples));

            for (size_t i = 0; i < std::size(samples); i++) {
                if (std::abs(samples[i] - sample_value) < 1e-6) {
                    n_ok++;
                } else {
                    n_ok = 0;
                }
            }

            ts += std::size(samples) / chan_count;
            sleep_for_n_samples(std::size(samples) / chan_count);
        }

        receiver_handler->OnStopIO();
    }

    std::cout << "\n";

    sender_stop = true;
    sender_thread.wait();
}
