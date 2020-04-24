//
// Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
//

#include <json_model/model.h>

#include <gtest/gtest.h>

class Model : json_model::Model {};
class NotModel {};

TEST(traits, traits) {
    static_assert(json_model::is_primitive_v<bool>);
    static_assert(json_model::is_primitive_v<double>);
    static_assert(json_model::is_primitive_v<int>);
    static_assert(json_model::is_primitive_v<int64_t>);
    static_assert(json_model::is_primitive_v<unsigned>);
    static_assert(json_model::is_primitive_v<uint64_t>);
    static_assert(json_model::is_primitive_v<std::string>);
    static_assert(json_model::is_primitive_v<std::nullptr_t>);
    static_assert(!json_model::is_primitive_v<float>);
    static_assert(!json_model::is_primitive_v<int16_t>);
    static_assert(!json_model::is_primitive_v<uint16_t>);
    static_assert(!json_model::is_primitive_v<char>);
    static_assert(!json_model::is_primitive_v<char*>);
    static_assert(!json_model::is_primitive_v<const char*>);

    static_assert(json_model::is_model_v<Model>);
    static_assert(!json_model::is_model_v<NotModel>);

    static_assert(json_model::is_pointer_v<std::unique_ptr<Model>>);
    static_assert(!json_model::is_pointer_v<std::unique_ptr<NotModel>>);
    static_assert(!json_model::is_pointer_v<std::unique_ptr<int>>);
    static_assert(!json_model::is_pointer_v<std::shared_ptr<Model>>);
    static_assert(!json_model::is_pointer_v<Model*>);

    static_assert(json_model::is_vector_v<std::vector<int>>);
    static_assert(json_model::is_vector_v<std::vector<std::unique_ptr<Model>>>);
    static_assert(json_model::is_vector_v<std::vector<std::vector<std::unique_ptr<Model>>>>);
    static_assert(!json_model::is_vector_v<std::vector<Model>>);

    static_assert(json_model::is_map_v<std::map<std::string, int>>);
    static_assert(json_model::is_map_v<std::map<std::string, std::unique_ptr<Model>>>);
    static_assert(!json_model::is_map_v<std::map<int, double>>);
    static_assert(!json_model::is_map_v<std::map<std::string, Model>>);

    static_assert(json_model::is_map_v<std::unordered_map<std::string, int>>);
    static_assert(json_model::is_map_v<std::unordered_map<std::string, std::unique_ptr<Model>>>);
    static_assert(!json_model::is_map_v<std::unordered_map<int, double>>);
    static_assert(!json_model::is_map_v<std::unordered_map<std::string, Model>>);

    static_assert(json_model::is_variant_v<std::variant<int>>);
    static_assert(json_model::is_variant_v<std::variant<int, double, std::string, std::unique_ptr<Model>, std::vector<std::nullptr_t>>>);
    static_assert(!json_model::is_variant_v<std::variant<>>);
    static_assert(!json_model::is_variant_v<std::variant<Model>>);
    static_assert(!json_model::is_variant_v<std::variant<std::variant<int>>>);
    static_assert(!json_model::is_variant_v<std::variant<std::unique_ptr<NotModel>>>);

    static_assert(!json_model::is_containable_v<std::optional<int>>);

    static_assert(json_model::is_optional_v<std::optional<int>>);
    static_assert(json_model::is_optional_v<std::optional<std::unique_ptr<Model>>>);
    static_assert(!json_model::is_optional_v<std::optional<std::optional<int>>>);

    static_assert(
        json_model::is_valid_field_v<
            std::optional<
                std::variant<
                    int,
                    double,
                    std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<std::unique_ptr<Model>>>>>>>,
                    std::map<std::string, std::map<std::string, std::vector<std::vector<std::variant<
                        std::unique_ptr<Model>,
                        std::vector<int>
                    >>>>>,
                    std::nullptr_t,
                    std::unique_ptr<Model>,
                    std::vector<std::unordered_map<std::string, int>>,
                    unsigned,
                    uint64_t,
                    int64_t,
                    bool,
                    std::string
                >
            >
        >
    );
}
