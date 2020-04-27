//
// Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
//

#include <json_model/error.h>

#include <gtest/gtest.h>

TEST(error, parse_error) {
    const std::string filler = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
    const size_t SEGMENT_SIZE = 30;

    {
        json_model::ParseError parse_error(filler, 5, "Some error");
        ASSERT_STREQ(
            parse_error.get_compact().c_str(),
            "Cannot parse json (offset 5): Some error"
        );
        ASSERT_EQ(
            parse_error.get_prettified(),
            "Cannot parse json (Some error at 5):\n"
            " | " + filler.substr(0, SEGMENT_SIZE * 2) + "...\n" +
            " | " + std::string(5, ' ') + "^"
        );
    }
    {
        json_model::ParseError parse_error(filler, filler.size() - 12, "Another error");
        ASSERT_EQ(
            parse_error.get_compact(),
            "Cannot parse json (offset " + std::to_string(filler.size() - 12) + "): Another error"
        );
        ASSERT_EQ(
            parse_error.get_prettified(),
            "Cannot parse json (Another error at " + std::to_string(filler.size() - 12) + "):\n" +
            " | ..." + filler.substr(filler.size() - SEGMENT_SIZE * 2) + "\n" +
            " | " + std::string(SEGMENT_SIZE * 2 - 12 + 3, ' ') + "^"
        );
    }
    {
        json_model::ParseError parse_error(filler, 50, "Third error");
        ASSERT_EQ(
            parse_error.get_compact(),
            "Cannot parse json (offset 50): Third error"
        );
        ASSERT_EQ(
            parse_error.get_prettified(),
            "Cannot parse json (Third error at 50):\n"
            " | ..." + filler.substr(50 - SEGMENT_SIZE, SEGMENT_SIZE * 2) + "...\n" +
            " | " + std::string(SEGMENT_SIZE + 3, ' ') + "^"
        );
    }
    {
        json_model::ParseError parse_error("Hello\nthere,\nworld!", 8, "Oh, no");
        ASSERT_EQ(
            parse_error.get_compact(),
            "Cannot parse json (offset 8): Oh, no"
        );
        ASSERT_EQ(
            parse_error.get_prettified(),
            std::string("Cannot parse json (Oh, no at 8):\n") +
            R"( | Hello\nthere,\nworld!)" + "\n" +
            R"( |          ^)"
        );
        ASSERT_STREQ(
            parse_error.what(),
            "Failed to parse json string"
        );
    }
}
