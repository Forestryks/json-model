//
// Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
//

#include <json_model/error.h>

#include <gtest/gtest.h>

namespace json_model::test_error {

////////////////////////////////////////////////////////////////////////////////

namespace parse_error {

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
        ASSERT_EQ(
            parse_error.get_offset_(),
            8u
        );
        ASSERT_EQ(
            parse_error.get_reason_(),
            "Oh, no"
        );
    }
}

} // namespace parse_error

////////////////////////////////////////////////////////////////////////////////

namespace type_string {

TEST(error, type_string) {
    ASSERT_STREQ(json_model::get_type_string(rapidjson::kNullType), "null");
    ASSERT_STREQ(json_model::get_type_string(rapidjson::kFalseType), "bool");
    ASSERT_STREQ(json_model::get_type_string(rapidjson::kTrueType), "bool");
    ASSERT_STREQ(json_model::get_type_string(rapidjson::kObjectType), "object");
    ASSERT_STREQ(json_model::get_type_string(rapidjson::kArrayType), "array");
    ASSERT_STREQ(json_model::get_type_string(rapidjson::kStringType), "string");
    ASSERT_STREQ(json_model::get_type_string(rapidjson::kNumberType), "number");
    int x = 20;
    ASSERT_STREQ(json_model::get_type_string(*reinterpret_cast<rapidjson::Type*>(&x)), "wtf");
}

} // namespace type_string

////////////////////////////////////////////////////////////////////////////////

namespace type_mismatch {

TEST(error, type_mismatch) {
    {
        json_model::TypeMismatchError string_string("string", "number");
        json_model::TypeMismatchError string_id("string", rapidjson::kNumberType);
        json_model::TypeMismatchError id_string(rapidjson::kStringType, "number");
        json_model::TypeMismatchError id_id(rapidjson::kStringType, rapidjson::kNumberType);
        ASSERT_EQ(
            string_string.get_compact(),
            string_id.get_compact()
        );
        ASSERT_EQ(
            string_string.get_compact(),
            id_string.get_compact()
        );
        ASSERT_EQ(
            string_string.get_compact(),
            id_id.get_compact()
        );
    }

    json_model::TypeMismatchError error("string", rapidjson::kNumberType);
    error.add_trace_index(123);
    error.add_trace_index(239);
    error.add_trace_key("hello");
    error.add_trace_index(179);
    error.add_trace_key("world");
    ASSERT_STREQ(
        error.what(),
        "Type mismatch"
    );
    ASSERT_EQ(
        error.get_compact(),
        R"(Type mismatch at 'root["world"][179]["hello"][239][123]' (expected: string, actual: number))"
    );
    ASSERT_EQ(
        error.get_prettified(),
        "Type mismatch:\n"
        "  expected: root[\"world\"][179][\"hello\"][239][123]\n"
        "  which is: number\n"
        "     to be: string"
    );
}

} // namespace type_mismatch

////////////////////////////////////////////////////////////////////////////////

namespace missing_key {

TEST(error, missing_key) {
    json_model::MissingKeyError error("id");
    error.add_trace_index(123);
    error.add_trace_index(239);
    error.add_trace_key("hello");
    error.add_trace_index(179);
    error.add_trace_key("world");
    ASSERT_STREQ(
        error.what(),
        "Missing required key"
    );
    ASSERT_EQ(
        error.get_compact(),
        R"(Key 'id' missing at 'root["world"][179]["hello"][239][123]')"
    );
    ASSERT_EQ(
        error.get_prettified(),
        "Missing required key:\n"
        "     expected: root[\"world\"][179][\"hello\"][239][123]\n"
        "  to have key: id"
    );
}

} // namespace missing_key

////////////////////////////////////////////////////////////////////////////////

} // namespace json_model::test_error
