//
// Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
//

#ifndef JSON_MODEL_INCLUDE_JSON_MODEL_INIT_H
#define JSON_MODEL_INCLUDE_JSON_MODEL_INIT_H

namespace json_model {

template<typename T>
void initialize(T& value) {
    if constexpr (is_primitive_v<T>) {
        value = T();
    } else if constexpr (is_pointer_v<T>) {
        value = std::make_unique<typename T::element_type>();
    } else if constexpr (is_variant_v<T>) {
        static_assert(std::variant_size_v<T> > 0);
        using variant_first_t = typename std::variant_alternative_t<0, T>;
        if constexpr (is_primitive_v<variant_first_t>) {
            value = variant_first_t();
        } else if constexpr (is_pointer_v<variant_first_t>) {
            value = std::make_unique<typename variant_first_t::element_type>();
        }
    }
}

} // namespace json_model

#endif // JSON_MODEL_INCLUDE_JSON_MODEL_INIT_H
