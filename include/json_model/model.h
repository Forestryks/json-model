//
// Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
//

#ifndef JSON_MODEL_INCLUDE_JSON_MODEL_MODEL_H
#define JSON_MODEL_INCLUDE_JSON_MODEL_MODEL_H

#include "traits.h"
#include "error.h"
#include "to_json.h"
#include "types.h"
#include "field.h"

#include "external/rapidjson/writer.h"
#include "external/rapidjson/stringbuffer.h"

// TODO: comparison functions
// TODO: clang-format
// TODO: encapsulate internal functions
// TODO: valgrind?

namespace json_model {

class Model {
public:
    [[nodiscard]] std::string to_json() const noexcept {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        to_json_internal(writer);
        writer.Flush();
        return buffer.GetString();
    }

    virtual void to_json_internal(json_writer_t& writer) const noexcept = 0;
//    virtual void from_json_internal(const json_value_t& value) = 0;
};

} // namespace json_model

#define PROVIDE_DETAILS(class_name, ...)\
static_assert(true); /* to ensure correct indentation when using code formatter */ \
public:\
    explicit class_name(json_model::ConstructorDummy _ = json_model::constructor_dummy) noexcept : __VA_ARGS__ {};\
    void to_json_internal(json_model::json_writer_t& _) const noexcept override {\
        _.StartObject();\
        __VA_ARGS__;\
        _.EndObject();\
    };\
//    void from_json_internal(const json_model::json_value_t& _) override {\
//        __VA_ARGS__;\
//    }

#endif // JSON_MODEL_INCLUDE_JSON_MODEL_MODEL_H
