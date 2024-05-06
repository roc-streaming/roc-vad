/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "driver/uid_generator.hpp"

#include <gtest/gtest.h>

#include <set>

using namespace rocvad;

struct UidGeneratorTest : testing::Test
{
    UidGenerator uid_gen;
};

TEST_F(UidGeneratorTest, generate)
{
    enum
    {
        UidCount = 100
    };

    std::set<std::string> uid_set;

    for (int i = 0; i < UidCount; i++) {
        auto uid = uid_gen.generate();

        ASSERT_FALSE(uid_set.count(uid));
        uid_set.insert(uid);
    }
}
