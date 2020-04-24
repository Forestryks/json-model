//
// Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
//

#ifndef JSON_MODEL_INCLUDE_JSON_MODEL_ERROR_H
#define JSON_MODEL_INCLUDE_JSON_MODEL_ERROR_H

#include <string>
#include <exception>

namespace json_model {

class Exception : public std::exception {
public:
    explicit Exception(const char* message) noexcept : std::exception(), msg_(message) {}
    explicit Exception(const std::string& message) noexcept : std::exception(), msg_(message) {}

    ~Exception() noexcept override = default;
    [[nodiscard]] const char* what() const noexcept override {
        return msg_.c_str();
    }

protected:
    std::string msg_;
};

} // namespace json_model

#endif // JSON_MODEL_INCLUDE_JSON_MODEL_ERROR_H
