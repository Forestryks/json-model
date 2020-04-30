//
// Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
//

#ifndef JSON_MODEL_INCLUDE_JSON_MODEL_FROM_JSON_H
#define JSON_MODEL_INCLUDE_JSON_MODEL_FROM_JSON_H

#include "types.h"
#include "traits.h"
#include "error.h"
#include "init.h"

#include "external/rapidjson/document.h"
#include <type_traits>

namespace json_model {

template<typename T>
typename std::enable_if_t<is_primitive_v<T>, bool>
from_json(const json_value_t& json_value, T& value, bool throw_on_error) {
    if constexpr (std::is_same_v<T, bool>) {
        if (!json_value.IsBool()) {
            if (throw_on_error) {
                throw TypeMismatchError("bool", json_value.GetType());
            }
            return false;
        }
        value = json_value.GetBool();
    } else if constexpr (std::is_same_v<T, double>) {
        if (!json_value.IsLosslessDouble()) {
            if (throw_on_error) {
                throw TypeMismatchError("double", json_value.GetType());
            }
            return false;
        }
        value = json_value.GetDouble();
    } else if constexpr (std::is_same_v<T, int>) {
        if (!json_value.IsInt()) {
            if (throw_on_error) {
                throw TypeMismatchError("int", json_value.GetType());
            }
            return false;
        }
        value = json_value.GetInt();
    } else if constexpr (std::is_same_v<T, int64_t>) {
        if (!json_value.IsInt64()) {
            if (throw_on_error) {
                throw TypeMismatchError("int64", json_value.GetType());
            }
            return false;
        }
        value = json_value.GetInt64();
    } else if constexpr (std::is_same_v<T, unsigned>) {
        if (!json_value.IsUint()) {
            if (throw_on_error) {
                throw TypeMismatchError("uint", json_value.GetType());
            }
            return false;
        }
        value = json_value.GetUint();
    } else if constexpr (std::is_same_v<T, uint64_t>) {
        if (!json_value.IsUint64()) {
            if (throw_on_error) {
                throw TypeMismatchError("uint64", json_value.GetType());
            }
            return false;
        }
        value = json_value.GetUint64();
    } else if constexpr (std::is_same_v<T, std::string>) {
        if (!json_value.IsString()) {
            if (throw_on_error) {
                throw TypeMismatchError("string", json_value.GetType());
            }
            return false;
        }
        value = json_value.GetString();
    } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
        if (!json_value.IsNull()) {
            if (throw_on_error) {
                throw TypeMismatchError("null", json_value.GetType());
            }
            return false;
        }
        value = nullptr;
    }
    return true;
}

template<typename T>
typename std::enable_if_t<is_pointer_v<T>, bool>
from_json(const json_value_t& json_value, T& value, bool throw_on_error) {
    return value->from_json_internal(json_value, throw_on_error);
}

template<typename T>
typename std::enable_if_t<is_map_v<T>, bool>
from_json(const json_value_t& json_value, T& value, bool throw_on_error);

template<typename T, size_t I = 0>
typename std::enable_if_t<is_variant_v<T>, bool>
from_json(const json_value_t& json_value, T& value, bool throw_on_error);

template<typename T>
typename std::enable_if_t<is_vector_v<T>, bool>
from_json(const json_value_t& json_value, T& value, bool throw_on_error) {
    if (!json_value.IsArray()) {
        if (throw_on_error) {
            throw TypeMismatchError("array", json_value.GetType());
        }
        return false;
    }
    value.clear();
    for (size_t i = 0; i < json_value.Size(); ++i) {
        typename T::value_type obj;
        initialize(obj);
        if (throw_on_error) {
            try {
                assert(from_json(json_value[i], obj, true));
            } catch (SchemaError& error) {
                error.add_trace_index(i);
                throw;
            }
        } else {
            if (!from_json(json_value[i], obj, false)) {
                return false;
            }
        }
        value.emplace_back(std::move(obj));
    }
    return true;
}

template<typename T>
typename std::enable_if_t<is_map_v<T>, bool>
from_json(const json_value_t& json_value, T& value, bool throw_on_error) {
    if (!json_value.IsObject()) {
        if (throw_on_error) {
            throw TypeMismatchError("object", json_value.GetType());
        }
        return false;
    }
    value.clear();
    for (auto iter = json_value.MemberBegin(); iter != json_value.MemberEnd(); ++iter) {
        typename T::mapped_type obj;
        initialize(obj);
        if (throw_on_error) {
            try {
                assert(from_json(iter->value, obj, true));
            } catch (SchemaError& error) {
                error.add_trace_key(iter->name.GetString());
                throw;
            }
        } else {
            if (!from_json(iter->value, obj, false)) {
                return false;
            }
        }
        value[iter->name.GetString()] = std::move(obj);
    }
    return true;
}

template<typename T, size_t I>
typename std::enable_if_t<is_variant_v<T>, bool>
from_json(const json_value_t& json_value, T& value, bool throw_on_error) {
    constexpr bool IsLast = (I + 1 == std::variant_size_v<T>);
    static_assert(std::variant_size_v<T> != 0);
    using V = typename std::variant_alternative_t<I, T>;
    value.template emplace<V>();
    initialize(std::get<V>(value));
    if constexpr (IsLast) {
        return from_json(json_value, std::get<V>(value), throw_on_error);
    } else {
        if (from_json(json_value, std::get<V>(value), false)) {
            return true;
        }
        return from_json<T, I + 1>(json_value, value, throw_on_error);
    }
}

//template<typename T>
//typename std::enable_if_t<is_variant_v<T>, bool>
//from_json(const json_value_t& json_value, T& value, bool throw_on_error) {
//    return variant_from_json(json_value, value, throw_on_error);
//}

} // namespace json_model

#endif // JSON_MODEL_INCLUDE_JSON_MODEL_FROM_JSON_H
