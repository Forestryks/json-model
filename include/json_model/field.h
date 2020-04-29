//
// Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
//

#ifndef JSON_MODEL_INCLUDE_JSON_MODEL_FIELD_H
#define JSON_MODEL_INCLUDE_JSON_MODEL_FIELD_H

#include "traits.h"
#include "types.h"
#include "init.h"

namespace json_model {

inline struct ConstructorDummy {} constructor_dummy;

class JsonValueWrapper {
public:
    JsonValueWrapper(const json_value_t& value, bool throw_on_error) noexcept
        : value_(value), throw_on_error_(throw_on_error), failed_(false) {}

    const json_value_t& get_value() const noexcept {
        return value_;
    }

    bool throw_on_error() const noexcept {
        return throw_on_error_;
    }

    bool is_failed() const noexcept {
        return failed_;
    }

    void fail() const noexcept {
        failed_ = true;
    }
private:
    const json_value_t& value_;
    bool throw_on_error_;
    mutable bool failed_;
};

template<typename T>
class Field {
public:
    Field() = delete;
    Field(ConstructorDummy, const char*) noexcept {
        static_assert(!std::is_pointer_v<T>, "Use std::unique_ptr instead of raw pointers");
        static_assert(is_valid_for_field_v<T>);

        initialize(value_);
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

    void operator()(const JsonValueWrapper& value_wrapper, const char* name) {
        if (value_wrapper.is_failed()) return;
        if (!value_wrapper.get_value().HasMember(name)) {
            if constexpr (is_optional_v<T>) {
                value_.reset();
            } else {
                value_wrapper.fail();
                if (value_wrapper.throw_on_error()) {
                    throw MissingKeyError(name);
                }
            }
            return;
        }
        const auto& json_value = value_wrapper.get_value()[name];

        if (value_wrapper.throw_on_error()) {
            try {
                if constexpr (is_optional_v<T>) {
                    value_.emplace();
                    initialize(value_.value());
                    from_json(json_value, value_.value(), true);
                } else {
                    from_json(json_value, value_, true);
                }
            } catch (SchemaError& error) {
                error.add_trace_key(name);
                throw;
            }
        } else {
            if constexpr (is_optional_v<T>) {
                value_.emplace();
                initialize(value_.value());
                if (!from_json(json_value, value_.value(), false)) {
                    value_wrapper.fail();
                    return;
                }
            } else {
                if (!from_json(json_value, value_, false)) {
                    value_wrapper.fail();
                    return;
                }
            }
        }
    }

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
