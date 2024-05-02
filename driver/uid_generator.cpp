/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "uid_generator.hpp"

namespace rocvad {

std::string UidGenerator::generate()
{
    for (;;) {
        auto uid = make_();

        if (uids_.count(uid)) {
            // can happen only in multiverse
            continue;
        }

        uids_.insert(uid);
        return uid;
    }
}

std::string UidGenerator::make_()
{
    constexpr int num_digits = 24;
    constexpr int num_groups = 4;

    constexpr const char* digits = "0123456789abcdef";

    std::uniform_int_distribution<int> dist(0, 15);

    std::string res;

    for (int g = 0; g < num_groups; g++) {
        if (g != 0) {
            res += "-";
        }
        for (int n = 0; n < num_digits / num_groups; n++) {
            res += digits[dist(rng_)];
        }
    }

    return res;
}

} // namespace rocvad
