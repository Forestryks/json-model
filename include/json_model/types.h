//
// Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
//

#ifndef JSON_MODEL_INCLUDE_JSON_MODEL_TYPES_H
#define JSON_MODEL_INCLUDE_JSON_MODEL_TYPES_H

#include "external/rapidjson/writer.h"
#include "external/rapidjson/stringbuffer.h"
#include "external/rapidjson/document.h"

namespace json_model {

using json_writer_t = rapidjson::Writer<rapidjson::StringBuffer>;
using json_value_t = rapidjson::Value;

} // namespace json_model

#endif // JSON_MODEL_INCLUDE_JSON_MODEL_TYPES_H
