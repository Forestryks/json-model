//
// Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
//

#include <json_model/error.h>

#include <gtest/gtest.h>

TEST(errors, errors) {
    json_model::Exception exception_from_c_string("This is error");
    json_model::Exception exception_from_cpp_string(std::string("This is error"));
    ASSERT_STREQ(exception_from_c_string.what(), exception_from_cpp_string.what());
}
