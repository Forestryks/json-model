//
// Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
//

#ifndef JSON_MODEL_INCLUDE_JSON_MODEL_FIELD_H
#define JSON_MODEL_INCLUDE_JSON_MODEL_FIELD_H

#include <iostream>
#include "traits.h"
#include "types.h"

namespace json_model {

inline struct ConstructorDummy {} constructor_dummy;

template<typename T>
class Field {
public:
    Field() = delete;
    Field(ConstructorDummy, const char*) noexcept {
        static_assert(!std::is_pointer_v<T>, "Use std::unique_ptr instead of raw pointers");
        static_assert(is_valid_field_v<T>);

        if constexpr (is_primitive_v<T>) {
            value_ = T();
        } else if constexpr (is_pointer_v<T>) {
            value_ = std::make_unique<typename T::element_type>();
        } else if constexpr (is_variant_v<T>) {
            static_assert(std::variant_size_v<T> > 0);
            using variant_first_t = typename std::variant_alternative_t<0, T>;
            if constexpr (is_primitive_v<variant_first_t>) {
                value_ = variant_first_t();
            } else if constexpr (is_pointer_v<variant_first_t>) {
                value_ = std::make_unique<typename variant_first_t::element_type>();
            }
        }
    }

    void operator()(json_writer_t& writer, const char* name) const noexcept {
        if constexpr (is_optional_v<T>) {
            if (value_.has_value()) {
                writer.Key(name);
                to_json(writer, value_.value());
            }
        } else {
            writer.Key(name);
            to_json(writer, value_);
        }
    }

//    void operator()([[maybe_unused]] const json_value_t& value, [[maybe_unused]] const char* name) {
//
//    }

    T value_;
};

#define DECLARE_FIELD(name, type, ...)\
static_assert(true); /* to ensure correct indentation when using code formatter */ \
private:\
    json_model::Field<type,##__VA_ARGS__> name;\
public:\
    type,##__VA_ARGS__& get_##name() noexcept {\
        return name.value_;\
    }\
    type,##__VA_ARGS__ const& get_##name() const noexcept {\
        return name.value_;\
    }\
    template <typename JsonModelT_, typename = typename std::enable_if_t<std::is_assignable_v<type,##__VA_ARGS__&, JsonModelT_>>>\
    auto& set_##name(JsonModelT_&& new_##name) noexcept(noexcept(name.value_ = std::forward<JsonModelT_>(new_##name))) {\
        name.value_ = std::forward<JsonModelT_>(new_##name);\
        return *this;\
    }

} // namespace json_model

#endif // JSON_MODEL_INCLUDE_JSON_MODEL_FIELD_H
