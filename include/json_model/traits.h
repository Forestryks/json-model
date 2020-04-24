//
// Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
//

#ifndef JSON_MODEL_INCLUDE_JSON_MODEL_TRAITS_H
#define JSON_MODEL_INCLUDE_JSON_MODEL_TRAITS_H

#include <type_traits>
#include <map>
#include <vector>
#include <variant>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <cstddef>

namespace json_model {
class Model;

template<typename T>
struct is_primitive : std::disjunction<
    std::is_same<T, bool>,
    std::is_same<T, double>,
    std::is_same<T, int>,
    std::is_same<T, int64_t>,
    std::is_same<T, unsigned>,
    std::is_same<T, uint64_t>,
    std::is_same<T, std::string>,
    std::is_same<T, nullptr_t>> {
};

template<typename T>
inline constexpr bool is_primitive_v = is_primitive<T>::value;

template<typename T>
struct is_model : std::is_base_of<Model, T> {};

template<typename T>
inline constexpr bool is_model_v = is_model<T>::value;

template<typename T>
struct is_pointer : std::false_type {};

template<typename T>
struct is_pointer<std::unique_ptr<T>> : is_model<T> {};

template<typename T>
inline constexpr bool is_pointer_v = is_pointer<T>::value;

template<typename T>
struct is_map : std::false_type {};

template<typename T>
struct is_vector : std::false_type {};

template<typename T>
struct is_variant : std::false_type {};

template<typename T>
struct is_containable : std::disjunction<
    is_primitive<T>,
    is_pointer<T>,
    is_map<T>,
    is_vector<T>,
    is_variant<T>
> {
};

template<typename T>
inline constexpr bool is_containable_v = is_containable<T>::value;

template<typename T>
struct is_map<std::map<std::string, T>> : is_containable<T> {};

template<typename T>
struct is_map<std::unordered_map<std::string, T>> : is_containable<T> {};

template<typename T>
struct is_vector<std::vector<T>> : is_containable<T> {};

template<typename Arg, typename... Args>
struct is_variant<std::variant<Arg, Args...>> :
    std::conjunction<
        std::disjunction<
            is_primitive<Arg>,
            is_pointer<Arg>,
            is_map<Arg>,
            is_vector<Arg>
        >,
        std::disjunction<
            is_primitive<Args>,
            is_pointer<Args>,
            is_map<Args>,
            is_vector<Args>
        >...
    > {
};

template<typename T>
inline constexpr bool is_map_v = is_map<T>::value;

template<typename T>
inline constexpr bool is_vector_v = is_vector<T>::value;

template<typename T>
inline constexpr bool is_variant_v = is_variant<T>::value;

template<typename T>
struct is_optional : std::false_type {};

template<typename T>
struct is_optional<std::optional<T>> : is_containable<T> {};

template<typename T>
inline constexpr bool is_optional_v = is_optional<T>::value;

template<typename T>
struct is_valid_field : std::disjunction<is_optional<T>, is_containable<T>> {};

template<typename T>
inline constexpr bool is_valid_field_v = is_valid_field<T>::value;

} // namespace json_model

#endif // JSON_MODEL_INCLUDE_JSON_MODEL_TRAITS_H
