/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "driver/log_manager.hpp"

#include <gtest/gtest.h>
#include <roc/log.h>
#include <spdlog/spdlog.h>

#include <cstring>

int main(int argc, char** argv)
{
    bool verbose = false;

    for (int i = 1; i < argc;) {
        if (strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
            for (int j = i; j < argc - 1; ++j) {
                argv[j] = argv[j + 1];
            }
            argc--;
        } else {
            i++;
        }
    }

    spdlog::set_level(verbose ? spdlog::level::trace : spdlog::level::off);
    spdlog::set_pattern("[%L%L] %v");

    roc_log_set_handler(rocvad::LogManager::roc_logger(), nullptr);
    roc_log_set_level(verbose ? ROC_LOG_DEBUG : ROC_LOG_NONE);

    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
