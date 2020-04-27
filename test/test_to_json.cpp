//
// Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
//

#include <json_model/model.h>

#include <gtest/gtest.h>
#include <functional>
#include <string>

////////////////////////////////////////////////////////////////////////////////

struct OnlyPrimitivesModel : public json_model::Model {
    DECLARE_FIELD(bool_field, bool);
    DECLARE_FIELD(double_field, double);
    DECLARE_FIELD(int_field, int);
    DECLARE_FIELD(int64_field, int64_t);
    DECLARE_FIELD(unsigned_field, unsigned);
    DECLARE_FIELD(uint64_field, uint64_t);
    DECLARE_FIELD(string_field, std::string);
    DECLARE_FIELD(nullptr_field, std::nullptr_t);

    PROVIDE_DETAILS(
        OnlyPrimitivesModel,
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

TEST(to_json, primitives) {
    OnlyPrimitivesModel model;
    model.set_bool_field(true);
    model.set_double_field(3.1415926);
    model.set_int_field(239);
    model.set_int64_field(INT64_MAX);
    model.set_unsigned_field(179u);
    model.set_uint64_field(UINT64_MAX);
    model.set_string_field("\"An algorithm must be seen to be believed.\" - Donald Knuth");
    model.set_nullptr_field(nullptr); // Unexpectedly
    ASSERT_STREQ(
        model.to_json().c_str(),
        R"({"bool-field":true,"double-field":3.1415926,"int-field":239,"int64-field":9223372036854775807,"unsigned-field":179,"uint64-field":18446744073709551615,"string-field":"\"An algorithm must be seen to be believed.\" - Donald Knuth","null-field":null})"
    );
}

////////////////////////////////////////////////////////////////////////////////

struct InnerNestedModel : public json_model::Model {
    DECLARE_FIELD(string, std::string);
    DECLARE_FIELD(null_field, std::nullptr_t);

    PROVIDE_DETAILS(
        InnerNestedModel,
        string(_, "string"),
        null_field(_, "null")
    )
};

struct NestedModel : public json_model::Model {
    DECLARE_FIELD(inner, std::unique_ptr<InnerNestedModel>);

    PROVIDE_DETAILS(
        NestedModel,
        inner(_, "inner")
    )
};

TEST(to_json, nested_model) {
    NestedModel model;
    model.get_inner()->set_string("hello").set_null_field(nullptr);
    ASSERT_STREQ(model.to_json().c_str(), R"({"inner":{"string":"hello","null":null}})");
}

////////////////////////////////////////////////////////////////////////////////

struct HighlyNestedModel : public json_model::Model {
    DECLARE_FIELD(nested, std::optional<std::unique_ptr<HighlyNestedModel>>);

    PROVIDE_DETAILS(
        HighlyNestedModel,
        nested(_, "nested")
    )
};

TEST(to_json, highly_nested_model) {
    HighlyNestedModel model;
    HighlyNestedModel* current_nested = &model;
    std::string prefix = "{";
    std::string suffix = "}";
    for (size_t i = 0; i < 100; ++i) {
        current_nested->get_nested() = std::make_unique<HighlyNestedModel>();
        current_nested = current_nested->get_nested()->get();
        prefix += R"("nested":{)";
        suffix += "}";
        ASSERT_EQ(model.to_json(), prefix + suffix);
    }
}

////////////////////////////////////////////////////////////////////////////////

struct InnerVectorModel : public json_model::Model {
    DECLARE_FIELD(value, std::string);

    PROVIDE_DETAILS(
        InnerVectorModel,
        value(_, "value")
    )
};

struct VectorModel : public json_model::Model {
    DECLARE_FIELD(simple_vector, std::vector<int>);
    DECLARE_FIELD(pointer_vector, std::vector<std::unique_ptr<InnerVectorModel>>);
    DECLARE_FIELD(map_vector, std::vector<std::map<std::string, int>>);
    DECLARE_FIELD(variant_vector, std::vector<std::variant<int, std::string>>);

    PROVIDE_DETAILS(
        VectorModel,
        simple_vector(_, "nums"),
        pointer_vector(_, "objects"),
        map_vector(_, "maps"),
        variant_vector(_, "variants")
    )
};

TEST(to_json, vector) {
    const int N = 1000;
    VectorModel model;
    std::string expected = R"({"nums":[)";
    for (int i = 1; i <= N; ++i) {
        expected += std::to_string(i);
        if (i == N) {
            expected += ']';
        } else {
            expected += ',';
        }

        model.get_simple_vector().push_back(i);
    }

    expected += R"(,"objects":[)";
    for (int i = N; i >= 1; --i) {
        expected += R"({"value":")" + std::to_string(i) + "\"}";
        if (i == 1) {
            expected += ']';
        } else {
            expected += ',';
        }

        auto ptr = std::make_unique<InnerVectorModel>();
        ptr->set_value(std::to_string(i));
        model.get_pointer_vector().emplace_back(std::move(ptr));
    }

    expected += R"(,"maps":[{"a":1,"b":2,"c":3},{"d":4,"e":5,"f":6},{"g":7,"h":8,"i":9}])";
    auto& maps = model.get_map_vector();
    maps = {
        {
            {"a", 1},
            {"b", 2},
            {"c", 3}
        },
        {
            {"d", 4},
            {"e", 5},
            {"f", 6}
        },
        {
            {"g", 7},
            {"h", 8},
            {"i", 9}
        }
    };

    expected += R"(,"variants":[239,"hello",179,"there"])";
    auto& variants = model.get_variant_vector();
    variants = {239, "hello", 179, "there"};

    expected += "}";
    ASSERT_EQ(model.to_json(), expected);
}

////////////////////////////////////////////////////////////////////////////////

struct InnerMapModel : public json_model::Model {
    DECLARE_FIELD(value, int);

    PROVIDE_DETAILS(
        InnerMapModel,
        value(_, "value")
    )
};

struct MapModel : public json_model::Model {
    DECLARE_FIELD(simple_map, std::map<std::string, int>);
    DECLARE_FIELD(pointer_map, std::map<std::string, std::unique_ptr<InnerMapModel>>);
    DECLARE_FIELD(vector_map, std::map<std::string, std::vector<int>>);
    DECLARE_FIELD(variant_map, std::map<std::string, std::variant<int, std::string>>);
    DECLARE_FIELD(nested_map, std::map<std::string, std::map<std::string, int>>);

    PROVIDE_DETAILS(
        MapModel,
        simple_map(_, "nums"),
        pointer_map(_, "objects"),
        vector_map(_, "vectors"),
        variant_map(_, "variants"),
        nested_map(_, "nested")
    )
};

struct UnorderedMapModel : public json_model::Model {
    DECLARE_FIELD(simple_map, std::unordered_map<std::string, int>);
    DECLARE_FIELD(pointer_map, std::unordered_map<std::string, std::unique_ptr<InnerMapModel>>);
    DECLARE_FIELD(vector_map, std::map<std::string, std::vector<int>>);
    DECLARE_FIELD(variant_map, std::map<std::string, std::variant<int, std::string>>);
    DECLARE_FIELD(nested_map, std::map<std::string, std::map<std::string, int>>);

    PROVIDE_DETAILS(
        UnorderedMapModel,
        simple_map(_, "nums"),
        pointer_map(_, "objects"),
        vector_map(_, "vectors"),
        variant_map(_, "variants"),
        nested_map(_, "nested")
    )
};

TEST(to_json, map) {
    const int N = 1000;
    MapModel model;
    UnorderedMapModel unordered_model;

    std::vector<std::pair<std::string, int>> ordering;
    for (int i = 1; i <= N; ++i) {
        ordering.push_back({std::to_string(i), i});
    }
    std::sort(ordering.begin(), ordering.end());

    std::string expected = R"({"nums":{)";
    for (size_t i = 0; i < ordering.size(); ++i) {
        expected += "\"" + ordering[i].first + "\":" + ordering[i].first;
        if (i + 1 == ordering.size()) {
            expected += '}';
        } else {
            expected += ',';
        }

        model.get_simple_map()[ordering[i].first] = ordering[i].second;
        unordered_model.get_simple_map()[ordering[i].first] = ordering[i].second;
    }

    expected += R"(,"objects":{)";
    for (size_t i = 0; i < ordering.size(); ++i) {
        expected += "\"" + ordering[i].first + R"(":{"value":)" + ordering[i].first + "}";
        if (i + 1 == ordering.size()) {
            expected += '}';
        } else {
            expected += ',';
        }

        auto ptr = std::make_unique<InnerMapModel>();
        ptr->set_value(ordering[i].second);
        model.get_pointer_map()[ordering[i].first] = std::move(ptr);

        ptr = std::make_unique<InnerMapModel>();
        ptr->set_value(ordering[i].second);
        unordered_model.get_pointer_map()[ordering[i].first] = std::move(ptr);
    }

    expected += R"(,"vectors":{"a":[1,2],"b":[0],"c":[3,4,5,6],"d":[7,8,9]})";
    model.get_vector_map() = unordered_model.get_vector_map() = {
        {"a", {1, 2}},
        {"b", {0}},
        {"c", {3, 4, 5, 6}},
        {"d", {7, 8, 9}}
    };

    expected += R"(,"variants":{"239":179,"hello":"there","ok":"google"})";
    model.get_variant_map() = unordered_model.get_variant_map() = {
        {"hello", "there"},
        {"239",   179},
        {"ok",    "google"}
    };

    expected += R"(,"nested":{"123":{"1":1,"2":2,"3":3},"hello":{"there":1,"world":2}})";
    model.get_nested_map() = unordered_model.get_nested_map() = {
        {"hello", {{"there", 1}, {"world", 2}}},
        {"123",   {{"1",     1}, {"2",     2}, {"3", 3}}}
    };

    expected += "}";
    ASSERT_EQ(model.to_json(), expected);

    // We can't expect any ordering from unordered_set, so we just check that character sets are same
    std::string ordered_string = model.to_json();
    std::string unordered_string = model.to_json();
    std::sort(ordered_string.begin(), ordered_string.end());
    std::sort(unordered_string.begin(), unordered_string.end());
    ASSERT_EQ(ordered_string, unordered_string);
}

////////////////////////////////////////////////////////////////////////////////

template<typename T, int Depth>
struct nested_vector {
public:
    using type = std::vector<typename nested_vector<T, Depth - 1>::type>;
};

template<typename T>
struct nested_vector<T, 1> {
public:
    using type = std::vector<T>;
};

template<typename T, int Depth>
using nested_vector_t = typename nested_vector<T, Depth>::type;

template<typename T, typename V>
void initialize_vector(T& vector, const V& gen) {
    vector = gen();
}

template<typename T, typename V>
void initialize_vector(std::vector<T>& vector, const V& gen) {
    vector.resize(2);
    initialize_vector(vector[0], gen);
    initialize_vector(vector[1], gen);
}

template<typename T, int Depth>
struct NestedVectorModel : public json_model::Model {
    DECLARE_FIELD(vector, nested_vector_t<T, Depth>);

    PROVIDE_DETAILS(
        NestedVectorModel,
        vector(_, "vector")
    )
};

TEST(to_json, nested_vector) {
    NestedVectorModel<int, 10> model;
    int val = 0;
    auto gen = [&val]() -> int {
        return val++;
    };
    initialize_vector(model.get_vector(), gen);

    val = 0;
    std::function<std::string(int)> get_expected = [&](int depth) -> std::string {
        if (depth == 0) {
            return std::to_string(gen());
        }
        std::string first = get_expected(depth - 1);
        std::string second = get_expected(depth - 1);
        return "[" + first + "," + second + "]";
    };
    std::string expected = "{\"vector\":" + get_expected(10) + "}";
    ASSERT_EQ(model.to_json(), expected);
}

////////////////////////////////////////////////////////////////////////////////

struct InnerVariantModel : json_model::Model {
    DECLARE_FIELD(string, std::string);

    PROVIDE_DETAILS(
        InnerVariantModel,
        string(_, "string")
    )
};

struct VariantModel : json_model::Model {
    DECLARE_FIELD(var, std::variant<
        std::unique_ptr<InnerVariantModel>,
        bool,
        int,
        std::string,
        std::nullptr_t,
        std::vector<int>,
        std::map<std::string, std::string>
    >);

    PROVIDE_DETAILS(
        VariantModel,
        var(_, "var")
    )
};

struct PrimitiveVariantModel : json_model::Model {
    DECLARE_FIELD(var, std::variant<int>);

    PROVIDE_DETAILS(
        PrimitiveVariantModel,
        var(_, "var")
    );
};

TEST(to_json, variant) {
    VariantModel model;
    ASSERT_EQ(model.get_var().index(), 0u);
    ASSERT_STREQ(model.to_json().c_str(), R"({"var":{"string":""}})");
    model.set_var(true);
    ASSERT_STREQ(model.to_json().c_str(), R"({"var":true})");
    model.set_var(239);
    ASSERT_STREQ(model.to_json().c_str(), R"({"var":239})");
    model.set_var(std::string("hello"));
    ASSERT_STREQ(model.to_json().c_str(), R"({"var":"hello"})");
    model.get_var().emplace<nullptr_t>(nullptr);
    ASSERT_STREQ(model.to_json().c_str(), R"({"var":null})");
    model.set_var(std::vector<int>{1, 2, 3, 4});
    ASSERT_STREQ(model.to_json().c_str(), R"({"var":[1,2,3,4]})");
    model.set_var(
        std::map<std::string, std::string>{
            {"hello", "there"},
            {"239",   "179"}
        }
    );
    ASSERT_STREQ(model.to_json().c_str(), R"({"var":{"239":"179","hello":"there"}})");
    auto ptr = std::make_unique<InnerVariantModel>();
    ptr->set_string("hello");
    model.get_var() = std::move(ptr);
    ASSERT_STREQ(model.to_json().c_str(), R"({"var":{"string":"hello"}})");

    PrimitiveVariantModel primitive_model;
    ASSERT_STREQ(primitive_model.to_json().c_str(), R"({"var":0})");
}

////////////////////////////////////////////////////////////////////////////////
