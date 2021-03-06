//
// Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
//

#ifndef JSON_MODEL_INCLUDE_JSON_MODEL_MODEL_H
#define JSON_MODEL_INCLUDE_JSON_MODEL_MODEL_H

#include "to_json.h"
#include "from_json.h"
#include "traits.h"
#include "error.h"
#include "types.h"
#include "field.h"

#include "external/rapidjson/writer.h"
#include "external/rapidjson/stringbuffer.h"
#include "external/rapidjson/error/en.h"

// TODO: comparison functions
// TODO: clang-format
// TODO: encapsulate internal functions
// TODO: valgrind?
// TODO: use std::string_view where possible

namespace json_model {

class Model {
public:
    virtual ~Model() noexcept = default;

    [[nodiscard]] std::string to_json() const noexcept {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        to_json_internal(writer);
        writer.Flush();
        return buffer.GetString();
    }

    bool from_json(const std::string& json_str, bool throw_on_error = true) {
        rapidjson::Document document;
        if (document.Parse(json_str.c_str()).HasParseError()) {
            if (throw_on_error) {
                throw ParseError(json_str, document.GetErrorOffset(), rapidjson::GetParseError_En(document.GetParseError()));
            }
            return false;
        }

        return from_json_internal(document, throw_on_error);
    }

    virtual void to_json_internal(json_writer_t& writer) const noexcept = 0;
    virtual bool from_json_internal(const json_value_t& value_wrapper, bool throw_on_error) = 0;
};

} // namespace json_model

#define PROVIDE_DETAILS(class_name, ...)\
static_assert(true); /* to ensure correct indentation when using code formatter */ \
public:\
    ~class_name() noexcept override = default;\
    explicit class_name(json_model::ConstructorDummy _ = json_model::constructor_dummy) noexcept : __VA_ARGS__ {};\
    void to_json_internal(json_model::json_writer_t& _) const noexcept override {\
        _.StartObject();\
        __VA_ARGS__;\
        _.EndObject();\
    };\
    bool from_json_internal(const json_model::json_value_t& json_value, bool throw_on_error) override {\
        if (!json_value.IsObject()) {\
            if (throw_on_error) {\
                throw json_model::TypeMismatchError(rapidjson::kObjectType, json_value.GetType());\
            }\
            return false;\
        }\
        json_model::JsonValueWrapper _(json_value, throw_on_error);\
        __VA_ARGS__;\
        return !_.is_failed();\
    }

#endif // JSON_MODEL_INCLUDE_JSON_MODEL_MODEL_H
