/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "tool/format.hpp"
#include "tool/parse.hpp"

#include <gtest/gtest.h>

using namespace rocvad;

struct FormatParseTest : testing::Test
{
};

TEST_F(FormatParseTest, parse_index)
{
    { // good
        uint32_t idx = 0;

        EXPECT_TRUE(parse_index("123456", idx));
        EXPECT_EQ(123456, idx);

        EXPECT_TRUE(parse_index("1", idx));
        EXPECT_EQ(1, idx);

        EXPECT_TRUE(parse_index("4294967295", idx));
        EXPECT_EQ(4294967295, idx);
    }

    { // bad
        uint32_t idx = 0;

        EXPECT_FALSE(parse_index("0", idx));
        EXPECT_FALSE(parse_index(" 10", idx));
        EXPECT_FALSE(parse_index("10 ", idx));
        EXPECT_FALSE(parse_index("-10", idx));
        EXPECT_FALSE(parse_index("4294967296", idx));
        EXPECT_FALSE(parse_index("99999999999999999", idx));
        EXPECT_FALSE(parse_index("BAD", idx));
    }
}

TEST_F(FormatParseTest, parse_duration)
{
    { // positive
        int64_t dur = 0;

        EXPECT_TRUE(parse_duration("--test", "0ns", dur));
        EXPECT_EQ(0, dur);

        EXPECT_TRUE(parse_duration("--test", "999ns", dur));
        EXPECT_EQ(999, dur);

        EXPECT_TRUE(parse_duration("--test", "999us", dur));
        EXPECT_EQ(999'000, dur);

        EXPECT_TRUE(parse_duration("--test", "999ms", dur));
        EXPECT_EQ(999'000'000, dur);

        EXPECT_TRUE(parse_duration("--test", "59s", dur));
        EXPECT_EQ(59'000'000'000, dur);

        EXPECT_TRUE(parse_duration("--test", "59m", dur));
        EXPECT_EQ(3540'000'000'000, dur);

        EXPECT_TRUE(parse_duration("--test", "100h", dur));
        EXPECT_EQ(360'000'000'000'000, dur);
    }

    { // negative
        int64_t dur = 0;

        EXPECT_TRUE(parse_duration("--test", "-0ns", dur));
        EXPECT_EQ(0, dur);

        EXPECT_TRUE(parse_duration("--test", "-999ns", dur));
        EXPECT_EQ(-999, dur);

        EXPECT_TRUE(parse_duration("--test", "-999us", dur));
        EXPECT_EQ(-999'000, dur);

        EXPECT_TRUE(parse_duration("--test", "-999ms", dur));
        EXPECT_EQ(-999'000'000, dur);

        EXPECT_TRUE(parse_duration("--test", "-59s", dur));
        EXPECT_EQ(-59'000'000'000, dur);

        EXPECT_TRUE(parse_duration("--test", "-59m", dur));
        EXPECT_EQ(-3540'000'000'000, dur);

        EXPECT_TRUE(parse_duration("--test", "-100h", dur));
        EXPECT_EQ(-360'000'000'000'000, dur);
    }

    { // bad
        int64_t dur = 0;

        EXPECT_FALSE(parse_duration("--test", "1", dur));
        EXPECT_FALSE(parse_duration("--test", " 1ms", dur));
        EXPECT_FALSE(parse_duration("--test", "1ms ", dur));
        EXPECT_FALSE(parse_duration("--test", "9999999999999999999999h", dur));
        EXPECT_FALSE(parse_duration("--test", "-9999999999999999999999h", dur));
        EXPECT_FALSE(parse_duration("--test", "BAD", dur));
    }
}

TEST_F(FormatParseTest, format_duration)
{
    { // positive
        EXPECT_EQ("default", format_duration(0));

        EXPECT_EQ("1ns", format_duration(1));
        EXPECT_EQ("999ns", format_duration(999));

        EXPECT_EQ("1us", format_duration(1'000));
        EXPECT_EQ("999us", format_duration(999'000));

        EXPECT_EQ("1ms", format_duration(1'000'000));
        EXPECT_EQ("999ms", format_duration(999'000'000));

        EXPECT_EQ("1s", format_duration(1'000'000'000));
        EXPECT_EQ("59s", format_duration(59'000'000'000));

        EXPECT_EQ("1m", format_duration(60'000'000'000));
        EXPECT_EQ("59m", format_duration(3540'000'000'000));

        EXPECT_EQ("1h", format_duration(3600'000'000'000));
        EXPECT_EQ("100h", format_duration(360'000'000'000'000));
    }

    { // negative
        EXPECT_EQ("default", format_duration(0));

        EXPECT_EQ("-1ns", format_duration(-1));
        EXPECT_EQ("-999ns", format_duration(-999));

        EXPECT_EQ("-1us", format_duration(-1'000));
        EXPECT_EQ("-999us", format_duration(-999'000));

        EXPECT_EQ("-1ms", format_duration(-1'000'000));
        EXPECT_EQ("-999ms", format_duration(-999'000'000));

        EXPECT_EQ("-1s", format_duration(-1'000'000'000));
        EXPECT_EQ("-59s", format_duration(-59'000'000'000));

        EXPECT_EQ("-1m", format_duration(-60'000'000'000));
        EXPECT_EQ("-59m", format_duration(-3540'000'000'000));

        EXPECT_EQ("-1h", format_duration(-3600'000'000'000));
        EXPECT_EQ("-100h", format_duration(-360'000'000'000'000));
    }
}
