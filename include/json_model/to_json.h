//
// Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
//

#ifndef JSON_MODEL_INCLUDE_JSON_MODEL_WRITE_JSON_H
#define JSON_MODEL_INCLUDE_JSON_MODEL_WRITE_JSON_H

#include "traits.h"
#include "types.h"

#include <type_traits>

namespace json_model {

template<typename T>
typename std::enable_if_t<is_primitive_v<T>>
to_json(json_writer_t& writer, const T& value) noexcept {
    if constexpr (std::is_same_v<T, bool>) {
        writer.Bool(value);
    } else if constexpr (std::is_same_v<T, double>) {
        writer.Double(value);
    } else if constexpr (std::is_same_v<T, int>) {
        writer.Int(value);
    } else if constexpr (std::is_same_v<T, int64_t>) {
        writer.Int64(value);
    } else if constexpr (std::is_same_v<T, unsigned>) {
        writer.Uint(value);
    } else if constexpr (std::is_same_v<T, uint64_t>) {
        writer.Uint64(value);
    } else if constexpr (std::is_same_v<T, std::string>) {
        writer.String(value.c_str(), value.size(), true);
    } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
        writer.Null();
    }
}

template<typename T>
typename std::enable_if_t<is_pointer_v<T>>
to_json(json_writer_t& writer, const T& value) noexcept {
    assert(value);
    value->to_json_internal(writer);
}

template<typename T>
typename std::enable_if_t<is_map_v<T>>
to_json(json_writer_t& writer, const T& value) noexcept;

template<typename T>
typename std::enable_if_t<is_variant_v<T>>
to_json(json_writer_t& writer, const T& value) noexcept;

template<typename T>
typename std::enable_if_t<is_vector_v<T>>
to_json(json_writer_t& writer, const T& value) noexcept {
    writer.StartArray();
    for (size_t i = 0; i < value.size(); ++i) {
        to_json(writer, value[i]);
    }
    writer.EndArray();
}

template<typename T>
typename std::enable_if_t<is_map_v<T>>
to_json(json_writer_t& writer, const T& value) noexcept {
    writer.StartObject();
    for (const auto& item : value) {
        writer.Key(item.first.c_str());
        to_json(writer, item.second);
    }
    writer.EndObject();
}

template<typename T>
typename std::enable_if_t<is_variant_v<T>>
to_json(json_writer_t& writer, const T& value) noexcept {
    assert(!value.valueless_by_exception());
    std::visit(
        [&writer](auto&& arg) noexcept {
            to_json(writer, arg);
        }, value
    );
}

} // namespace json_model

#endif // JSON_MODEL_INCLUDE_JSON_MODEL_WRITE_JSON_H
