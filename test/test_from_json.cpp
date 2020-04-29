//
// Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
//

#include <json_model/model.h>

#include <gtest/gtest.h>

#define JSON_MODEL_THROWS_(type, ...) \
    try {\
        __VA_ARGS__;\
        FAIL() << "Expected exception";\
    } catch (type& error) {\
        throw;\
    } catch (std::exception& e) {\
        FAIL() << "Expected exception of type " #type ". what(): " << e.what();\
    } catch (...) {\
        FAIL() << "Expected exception of type " #type;\
    }

#define TEST_TYPE_MISMATCH(model_type, json_str, trace, expected, actual) do {\
    model_type model;\
    try {\
        JSON_MODEL_THROWS_(json_model::TypeMismatchError, model.from_json(json_str));\
    } catch (json_model::Exception& error) {\
        ASSERT_EQ(\
            error.get_compact(),\
            "Type mismatch at '" #trace "' (expected: " #expected ", actual: " #actual ")"\
        );\
    }\
    ASSERT_FALSE(model.from_json(json_str, false));\
} while (0)

#define TEST_CORRECT(model_type, json_str, ...) do {\
    model_type model;\
    ASSERT_TRUE(model.from_json(json_str));\
    {\
        __VA_ARGS__;\
    }\
    ASSERT_TRUE(model.from_json(json_str, false));\
    {\
        __VA_ARGS__;\
    }\
} while (0)

#define TEST_KEY_MISSING(model_type, json_str, trace, key) do {\
    model_type model;\
    try {\
        JSON_MODEL_THROWS_(json_model::MissingKeyError, model.from_json(json_str));\
    } catch (json_model::Exception& error) {\
        ASSERT_EQ(\
            error.get_compact(),\
            "Key '" #key "' missing at '" #trace "'"\
        );\
    }\
    ASSERT_FALSE(model.from_json(json_str, false));\
} while (0)

namespace json_model::test_from_json {

////////////////////////////////////////////////////////////////////////////////

namespace parse_error {

struct Model : public json_model::Model {
    DECLARE_FIELD(string, std::string);

    PROVIDE_DETAILS(
        Model,
        string(_, "string")
    )
};

TEST(from_json, parse_error) {
    std::string json_str = R"({"bad_json":})";

    Model model;

    try {
        JSON_MODEL_THROWS_(json_model::ParseError, model.from_json(json_str));
    } catch (json_model::Exception& error) {
        ASSERT_EQ(
            error.get_compact(),
            "Cannot parse json (offset 12): Invalid value."
        );
    }

    ASSERT_FALSE(model.from_json(json_str, false));
}

} // namespace parse_error

////////////////////////////////////////////////////////////////////////////////

namespace primitives {

struct Model : public json_model::Model {
    DECLARE_FIELD(bool_field, bool);
    DECLARE_FIELD(double_field, double);
    DECLARE_FIELD(int_field, int);
    DECLARE_FIELD(int64_field, int64_t);
    DECLARE_FIELD(unsigned_field, unsigned);
    DECLARE_FIELD(uint64_field, uint64_t);
    DECLARE_FIELD(string_field, std::string);
    DECLARE_FIELD(nullptr_field, std::nullptr_t);

    PROVIDE_DETAILS(
        Model,
        bool_field(_, "bool-field"),
        double_field(_, "double-field"),
        int_field(_, "int-field"),
        int64_field(_, "int64-field"),
        unsigned_field(_, "unsigned-field"),
        uint64_field(_, "uint64-field"),
        string_field(_, "string-field"),
        nullptr_field(_, "null-field")
    );
};

TEST(from_json, primitives) {
    TEST_CORRECT(
        Model,
        R"({"bool-field":true,"double-field":3.14159,"int-field":239,"int64-field":9223372036854775807,"unsigned-field":179,"uint64-field":18446744073709551615,"string-field":"\"An algorithm must be seen to be believed.\" - Donald Knuth","null-field":null})",
        {
            ASSERT_EQ(model.get_bool_field(), true);
            ASSERT_NEAR(model.get_double_field(), 3.14159, 1e-9);
            ASSERT_EQ(model.get_int_field(), 239);
            ASSERT_EQ(model.get_int64_field(), INT64_MAX);
            ASSERT_EQ(model.get_unsigned_field(), 179u);
            ASSERT_EQ(model.get_uint64_field(), UINT64_MAX);
            ASSERT_EQ(model.get_string_field(), "\"An algorithm must be seen to be believed.\" - Donald Knuth");
            ASSERT_EQ(model.get_nullptr_field(), nullptr);
        }
    );

    TEST_CORRECT(
        Model,
        R"({"bool-field":true,"double-field":3,"int-field":239,"int64-field":9223372036854775807,"unsigned-field":179,"uint64-field":18446744073709551615,"string-field":"\"An algorithm must be seen to be believed.\" - Donald Knuth","null-field":null})",
        {
            ASSERT_NEAR(model.get_double_field(), 3, 1e-9);
        }
    );

    TEST_TYPE_MISMATCH(
        Model,
        R"({"bool-field":1,"double-field":3.14159,"int-field":239,"int64-field":9223372036854775807,"unsigned-field":179,"uint64-field":18446744073709551615,"string-field":"some string","null-field":null})",
        root["bool-field"],
        bool,
        number
    );

    TEST_TYPE_MISMATCH(
        Model,
        R"({"bool-field":true,"double-field":true,"int-field":239,"int64-field":9223372036854775807,"unsigned-field":179,"uint64-field":18446744073709551615,"string-field":"some string","null-field":null})",
        root["double-field"],
        double,
        bool
    );

    TEST_TYPE_MISMATCH(
        Model,
        R"({"bool-field":true,"double-field":3.14159,"int-field":{},"int64-field":9223372036854775807,"unsigned-field":179,"uint64-field":18446744073709551615,"string-field":"some string","null-field":null})",
        root["int-field"],
        int,
        object
    );

    TEST_TYPE_MISMATCH(
        Model,
        R"({"bool-field":true,"double-field":3.14159,"int-field":239,"int64-field":[],"unsigned-field":179,"uint64-field":18446744073709551615,"string-field":"some string","null-field":null})",
        root["int64-field"],
        int64,
        array
    );

    TEST_TYPE_MISMATCH(
        Model,
        R"({"bool-field":true,"double-field":3.14159,"int-field":239,"int64-field":9223372036854775807,"unsigned-field":false,"uint64-field":18446744073709551615,"string-field":"some string","null-field":null})",
        root["unsigned-field"],
        uint,
        bool
    );

    TEST_TYPE_MISMATCH(
        Model,
        R"({"bool-field":true,"double-field":3.14159,"int-field":239,"int64-field":9223372036854775807,"unsigned-field":179,"uint64-field":"oh","string-field":"some string","null-field":null})",
        root["uint64-field"],
        uint64,
        string
    );

    TEST_TYPE_MISMATCH(
        Model,
        R"({"bool-field":true,"double-field":3.14159,"int-field":239,"int64-field":9223372036854775807,"unsigned-field":179,"uint64-field":18446744073709551615,"string-field":null,"null-field":null})",
        root["string-field"],
        string,
        null
    );

    TEST_TYPE_MISMATCH(
        Model,
        R"({"bool-field":true,"double-field":3.14159,"int-field":239,"int64-field":9223372036854775807,"unsigned-field":179,"uint64-field":18446744073709551615,"string-field":"some string","null-field":{"x":"y"}})",
        root["null-field"],
        null,
        object
    );
}

} // namespace primitives

////////////////////////////////////////////////////////////////////////////////

namespace nested_model {

struct InnerModel : public json_model::Model {
    DECLARE_FIELD(string, std::string);
    DECLARE_FIELD(null_field, std::nullptr_t);

    PROVIDE_DETAILS(
        InnerModel,
        string(_, "string"),
        null_field(_, "null")
    )
};

struct Model : public json_model::Model {
    DECLARE_FIELD(inner, std::unique_ptr<InnerModel>);

    PROVIDE_DETAILS(
        Model,
        inner(_, "inner")
    )
};

TEST(from_json, nested_model) {
    TEST_CORRECT(
        Model,
        R"({"inner":{"string":"hello","null":null}})",
        {
            ASSERT_TRUE(model.get_inner());
            ASSERT_EQ(model.get_inner()->get_string(), "hello");
            ASSERT_EQ(model.get_inner()->get_null_field(), nullptr);
        }
    );

    TEST_TYPE_MISMATCH(
        Model,
        R"({"inner":[{"string":"hello","null":null}]})",
        root["inner"],
        object,
        array
    );

    TEST_KEY_MISSING(
        Model,
        R"({})",
        root,
        inner
    );
}

} // namespace nested_model

////////////////////////////////////////////////////////////////////////////////

namespace optional {

struct InnerModel : public json_model::Model {
    DECLARE_FIELD(string_field, std::string);

    PROVIDE_DETAILS(
        InnerModel,
        string_field(_, "string")
    )
};

struct Model : public json_model::Model {
    DECLARE_FIELD(int_field, std::optional<int>);
    DECLARE_FIELD(nested_field, std::optional<std::unique_ptr<InnerModel>>);

    PROVIDE_DETAILS(
        Model,
        int_field(_, "int"),
        nested_field(_, "nested")
    )
};

TEST(from_json, optional) {
    TEST_CORRECT(
        Model,
        R"({})",
        {
            ASSERT_FALSE(model.get_int_field().has_value());
            ASSERT_FALSE(model.get_nested_field().has_value());
        }
    );

    TEST_CORRECT(
        Model,
        R"({"int":12})",
        {
            ASSERT_TRUE(model.get_int_field().has_value());
            ASSERT_EQ(model.get_int_field(), 12);
            ASSERT_FALSE(model.get_nested_field().has_value());
        }
    );

    TEST_CORRECT(
        Model,
        R"({"nested":{"string":"hello"}})",
        {
            ASSERT_FALSE(model.get_int_field().has_value());
            ASSERT_TRUE(model.get_nested_field().has_value());
            ASSERT_TRUE(model.get_nested_field().value());
            ASSERT_EQ(model.get_nested_field().value()->get_string_field(), "hello");
        }
    );

    TEST_TYPE_MISMATCH(
        Model,
        R"({"int":"string"})",
        root["int"],
        int,
        string
    );

    TEST_TYPE_MISMATCH(
        Model,
        R"({"nested":[]})",
        root["nested"],
        object,
        array
    );

    TEST_TYPE_MISMATCH(
        Model,
        R"({"nested":{"string":123}})",
        root["nested"]["string"],
        string,
        number
    );
}

} // namespace optional

////////////////////////////////////////////////////////////////////////////////

namespace vector {

struct InnerModel : public json_model::Model {
    DECLARE_FIELD(value, std::string);

    PROVIDE_DETAILS(
        InnerModel,
        value(_, "value")
    );
};

struct Model : public json_model::Model {
    DECLARE_FIELD(simple_vector, std::vector<int>);
    DECLARE_FIELD(pointer_vector, std::vector<std::unique_ptr<InnerModel>>);
    DECLARE_FIELD(map_vector, std::vector<std::map<std::string, int>>);
    DECLARE_FIELD(variant_vector, std::vector<std::variant<int, std::string>>);

    PROVIDE_DETAILS(
        Model,
        simple_vector(_, "nums"),
        pointer_vector(_, "objects"),
        map_vector(_, "maps"),
        variant_vector(_, "variants")
    )
};

TEST(from_json, vector) {
    TEST_CORRECT(
        Model,
        R"({"nums":[1,2,3],"objects":[{"value":"hello"}, {"value":"there"}],"maps":[{"a":1,"b":2},{"1":2,"2":1}],"variants":[179,"hello",239,"world"]})",
        {
            ASSERT_EQ(model.get_simple_vector(), (std::vector<int>{1, 2, 3}));

            ASSERT_EQ(model.get_pointer_vector().size(), 2u);
            ASSERT_EQ(model.get_pointer_vector()[0]->get_value(), "hello");
            ASSERT_EQ(model.get_pointer_vector()[1]->get_value(), "there");

            ASSERT_EQ(model.get_map_vector().size(), 2u);
            ASSERT_EQ(model.get_map_vector()[0], (std::map<std::string, int>{{"a", 1}, {"b", 2}}));
            ASSERT_EQ(model.get_map_vector()[1], (std::map<std::string, int>{{"1", 2}, {"2", 1}}));

            ASSERT_EQ(model.get_variant_vector(), (std::vector<std::variant<int, std::string>>{179, "hello", 239, "world"}));
        }
    );

    TEST_KEY_MISSING(
        Model,
        R"({"nums":[1,2,3],"objects":[{"value":"hello"}, {"not value":"there"}],"maps":[{"a":1,"b":2},{"1":2,"2":1}],"variants":[179,"hello",239,"world"]})",
        root["objects"][1],
        value
    );

    TEST_TYPE_MISMATCH(
        Model,
        R"({"nums":[1,2,3],"objects":[{"value":"hello"}, {"value":null}],"maps":[{"a":1,"b":2},{"1":2,"2":1}],"variants":[179,"hello",239,"world"]})",
        root["objects"][1]["value"],
        string,
        null
    );

    TEST_TYPE_MISMATCH(
        Model,
        R"({"nums":{},"objects":[{"value":"hello"}, {"value":"hi"}],"maps":[{"a":1,"b":2},{"1":2,"2":1}],"variants":[179,"hello",239,"world"]})",
        root["nums"],
        array,
        object
    );
}

} // namespace vector

////////////////////////////////////////////////////////////////////////////////

namespace map {

struct InnerModel : public json_model::Model {
    DECLARE_FIELD(value, int);

    PROVIDE_DETAILS(
        InnerModel,
        value(_, "value")
    )
};

struct Model : public json_model::Model {
    DECLARE_FIELD(simple_map, std::map<std::string, int>);
    DECLARE_FIELD(pointer_map, std::map<std::string, std::unique_ptr<InnerModel>>);
    DECLARE_FIELD(vector_map, std::map<std::string, std::vector<int>>);
    DECLARE_FIELD(variant_map, std::map<std::string, std::variant<std::string, int>>);
    DECLARE_FIELD(nested_map, std::map<std::string, std::map<std::string, int>>);

    PROVIDE_DETAILS(
        Model,
        simple_map(_, "nums"),
        pointer_map(_, "objects"),
        vector_map(_, "vectors"),
        variant_map(_, "variants"),
        nested_map(_, "nested")
    )
};

TEST(from_json, map) {
    TEST_CORRECT(
        Model,
        R"({"nums":{"a":1,"b":2,"c":3},"objects":{"first":{"value":123},"second":{"value":321}},"vectors":{"one":[1,1,1],"two":[2,2,2]},"variants":{"who":"me","why":42},"nested":{"hi":{"h":1,"i":2},"bye":{"b":1,"y":2,"e":3}}})",
        {
            ASSERT_EQ(model.get_simple_map(), (std::map<std::string, int>{{"a", 1}, {"b", 2}, {"c", 3}}));

            ASSERT_EQ(model.get_pointer_map().size(), 2u);
            ASSERT_EQ(model.get_pointer_map()["first"]->get_value(), 123);
            ASSERT_EQ(model.get_pointer_map()["second"]->get_value(), 321);

            ASSERT_EQ(model.get_vector_map(), (std::map<std::string, std::vector<int>>{{"one", {1, 1, 1}}, {"two", {2, 2, 2}}}));
            ASSERT_EQ(model.get_variant_map(), (std::map<std::string, std::variant<std::string, int>>{{"who", "me"}, {"why", 42}}));

            ASSERT_EQ(model.get_nested_map(), (std::map<std::string, std::map<std::string, int>>{{"hi", {{"h", 1}, {"i", 2}}}, {"bye", {{"b", 1}, {"y", 2}, {"e", 3}}}}));
        }
    );

    TEST_KEY_MISSING(
        Model,
        R"({"nums":{"a":1,"b":2,"c":3},"objects":{"first":{"value":123},"second":{"not value":321}},"vectors":{"one":[1,1,1],"two":[2,2,2]},"variants":{"who":"me","why":42},"nested":{"hi":{"h":1,"i":2},"bye":{"b":1,"y":2,"e":3}}})",
        root["objects"]["second"],
        value
    );

    TEST_TYPE_MISMATCH(
        Model,
        R"({"nums":{"a":1,"b":2,"c":3},"objects":{"first":{"value":true},"second":{"value":321}},"vectors":{"one":[1,1,1],"two":[2,2,2]},"variants":{"who":"me","why":42},"nested":{"hi":{"h":1,"i":2},"bye":{"b":1,"y":2,"e":3}}})",
        root["objects"]["first"]["value"],
        int,
        bool
    );

    TEST_TYPE_MISMATCH(
        Model,
        R"({"nums":[1,2,3],"objects":{"first":{"value":true},"second":{"value":321}},"vectors":{"one":[1,1,1],"two":[2,2,2]},"variants":{"who":"me","why":42},"nested":{"hi":{"h":1,"i":2},"bye":{"b":1,"y":2,"e":3}}})",
        root["nums"],
        object,
        array
    );
}

} // namespace map

////////////////////////////////////////////////////////////////////////////////

namespace variant {

struct InnerModel : public json_model::Model {
    DECLARE_FIELD(string, std::string);

    PROVIDE_DETAILS(
        InnerModel,
        string(_, "string")
    );
};

struct LastModel : public json_model::Model {
    DECLARE_FIELD(null, std::nullptr_t);

    PROVIDE_DETAILS(
        LastModel,
        null(_, "null")
    )
};

class Model : public json_model::Model {
    DECLARE_FIELD(
        var,
        std::variant<
            std::unique_ptr<InnerModel>,
            bool,
            int,
            std::string,
            std::nullptr_t,
            std::vector<int>,
            std::map<std::string, std::string>,
            std::unique_ptr<LastModel>
        >
    );

    PROVIDE_DETAILS(
        Model,
        var(_, "var")
    );
};

TEST(from_json, variant) {
    TEST_CORRECT(
        Model,
        R"({"var":{"string":"hello"}})",
        {
            ASSERT_EQ(model.get_var().index(), 0u);
            ASSERT_TRUE(std::get<0>(model.get_var()));
            ASSERT_EQ(std::get<0>(model.get_var())->get_string(), "hello");
        }
    );

    TEST_CORRECT(
        Model,
        R"({"var":true})",
        {
            ASSERT_EQ(model.get_var().index(), 1u);
            ASSERT_TRUE(std::get<1>(model.get_var()));
        }
    );

    TEST_CORRECT(
        Model,
        R"({"var":239})",
        {
            ASSERT_EQ(model.get_var().index(), 2u);
            ASSERT_EQ(std::get<2>(model.get_var()), 239);
        }
    );

    TEST_CORRECT(
        Model,
        R"({"var":"hello"})",
        {
            ASSERT_EQ(model.get_var().index(), 3u);
            ASSERT_EQ(std::get<3>(model.get_var()), "hello");
        }
    );

    TEST_CORRECT(
        Model,
        R"({"var":null})",
        {
            ASSERT_EQ(model.get_var().index(), 4u);
            ASSERT_EQ(std::get<4>(model.get_var()), nullptr);
        }
    );

    TEST_CORRECT(
        Model,
        R"({"var":[1, 7, 9]})",
        {
            ASSERT_EQ(model.get_var().index(), 5u);
            ASSERT_EQ(std::get<5>(model.get_var()), (std::vector<int>{1, 7, 9}));
        }
    );

    TEST_CORRECT(
        Model,
        R"({"var":{"hi":"hello","bye":"goodbye"}})",
        {
            ASSERT_EQ(model.get_var().index(), 6u);
            ASSERT_EQ(std::get<6>(model.get_var()), (std::map<std::string, std::string>{{"hi", "hello"}, {"bye", "goodbye"}}));
        }
    );

    TEST_CORRECT(
        Model,
        R"({"var":{"null":null}})",
        {
            ASSERT_EQ(model.get_var().index(), 7u);
            ASSERT_TRUE(std::get<7>(model.get_var()));
            ASSERT_EQ(std::get<7>(model.get_var())->get_null(), nullptr);
        }
    );

    TEST_TYPE_MISMATCH(
        Model,
        R"({"var":["oh"]})",
        root["var"]["<variant:7>"],
        object,
        array
    );

    TEST_KEY_MISSING(
        Model,
        R"({"var":{"gg":123}})",
        root["var"]["<variant:7>"],
        null
    );

    TEST_TYPE_MISMATCH(
        Model,
        R"({"var":{"null":123}})",
        root["var"]["<variant:7>"]["null"],
        null,
        number
    );
}

} // namespace variant

////////////////////////////////////////////////////////////////////////////////

} // namespace json_model::test_from_json
