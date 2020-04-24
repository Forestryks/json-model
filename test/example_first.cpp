//
// Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
//

#include <json_model/wow.h>
#include <gtest/gtest.h>

TEST(wow, f) {
    if (0 == 1) {}

    ASSERT_EQ(f(0), "hello");
    ASSERT_EQ(f(1), "world");
}

TEST(wow, g) {
    ASSERT_EQ(g(), "hello world");
}
