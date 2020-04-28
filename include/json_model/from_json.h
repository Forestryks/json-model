//
// Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
//

#ifndef JSON_MODEL_INCLUDE_JSON_MODEL_FROM_JSON_H
#define JSON_MODEL_INCLUDE_JSON_MODEL_FROM_JSON_H

#include "external/rapidjson/document.h"

namespace json_model {

class JsonValueWrapper {
public:
    JsonValueWrapper(const rapidjson::Value& value, bool throw_on_error) noexcept
        : value_(value), throw_on_error_(throw_on_error) {}

    const rapidjson::Value& get_value() {
        return value_;
    }

    bool throw_on_error() {
        return throw_on_error_;
    }
private:
    const rapidjson::Value& value_;
    bool throw_on_error_;
};



} // namespace json_model

#endif // JSON_MODEL_INCLUDE_JSON_MODEL_FROM_JSON_H
