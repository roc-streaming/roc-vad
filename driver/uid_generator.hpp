/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <random>
#include <string>
#include <unordered_set>

namespace rocvad {

// Generates unique device identifiers.
class UidGenerator
{
public:
    UidGenerator() = default;

    UidGenerator(const UidGenerator&) = delete;
    UidGenerator& operator=(const UidGenerator&) = delete;

    std::string generate();

private:
    std::string make_();

    std::random_device device_;
    std::mt19937 rng_ {device_()};

    std::unordered_set<std::string> uids_;
};

} // namespace rocvad
