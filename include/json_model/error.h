//
// Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
//

#ifndef JSON_MODEL_INCLUDE_JSON_MODEL_ERROR_H
#define JSON_MODEL_INCLUDE_JSON_MODEL_ERROR_H

#include "external/rapidjson/document.h"

#include <string>
#include <exception>
#include <vector>

namespace json_model {

class Exception : public std::exception {
public:
    ~Exception() noexcept override = default;

    virtual std::string get_compact() const noexcept = 0;
    virtual std::string get_prettified() const noexcept = 0;

protected:
    Exception() noexcept: std::exception() {}
};

class ParseError : public Exception {
public:
    ParseError(const std::string& json_str, size_t offset, const std::string& reason) noexcept
        : Exception(), offset_(offset), reason_(reason) {
        size_t segment_start, segment_end;
        size_t available_at_left = offset;
        size_t available_at_right = json_str.size() - offset;

        if (available_at_left < SEGMENT_SIZE) {
            segment_start = 0;
            segment_end = std::min(segment_start + SEGMENT_SIZE * 2, json_str.size());
        } else if (available_at_right < SEGMENT_SIZE) {
            segment_end = json_str.size();
            segment_start = std::max(SEGMENT_SIZE * 2, segment_end) - SEGMENT_SIZE * 2;
        } else {
            segment_start = offset - SEGMENT_SIZE;
            segment_end = offset + SEGMENT_SIZE;
        }
        local_offset_ = offset - segment_start;

        for (size_t i = segment_start; i < segment_end; ++i) {
            if (json_str[i] == '\n') {
                if (i < offset) {
                    local_offset_++;
                }
                segment_ += "\\n";
            } else {
                segment_ += json_str[i];
            }
        }

        if (segment_start != 0) {
            segment_ = "..." + segment_;
            local_offset_ += 3;
        }
        if (segment_end != json_str.size()) {
            segment_ += "...";
        }
    }

    ~ParseError() noexcept override = default;

    const char* what() const noexcept override {
        return "Failed to parse json string";
    }

    std::string get_compact() const noexcept override {
        return "Cannot parse json (offset " + std::to_string(offset_) + "): " + reason_;
    }

    std::string get_prettified() const noexcept override {
        return "Cannot parse json (" + reason_ + " at " + std::to_string(offset_) + "):\n" +
               " | " + segment_ + "\n" +
               " | " + std::string(local_offset_, ' ') + "^";
    }

    size_t get_offset_() const noexcept {
        return offset_;
    }

    const std::string& get_reason_() const noexcept {
        return reason_;
    }

private:
    std::string segment_;
    size_t offset_;
    size_t local_offset_;
    std::string reason_;

    const inline static size_t SEGMENT_SIZE = 30;
};

inline const char* get_type_string(rapidjson::Type type) noexcept {
    switch (type) {
        case rapidjson::Type::kNullType:
            return "null";
        case rapidjson::Type::kFalseType:
            [[fallthrough]];
        case rapidjson::Type::kTrueType:
            return "bool";
        case rapidjson::Type::kObjectType:
            return "object";
        case rapidjson::Type::kArrayType:
            return "array";
        case rapidjson::Type::kStringType:
            return "string";
        case rapidjson::Type::kNumberType:
            return "number";
    }
    return "wtf";
}

class SchemaError : public Exception {
public:
    SchemaError() noexcept: Exception() {}
    ~SchemaError() noexcept override = default;

    void add_trace_index(size_t index) noexcept {
        trace_.push_back(std::to_string(index));
    }

    void add_trace_key(const std::string& key) noexcept {
        trace_.push_back("\"" + key + "\"");
    }

    std::vector<std::string> get_trace() const noexcept {
        return std::vector<std::string>(trace_.rbegin(), trace_.rend());
    }

protected:
    std::string build_trace() const noexcept {
        std::string result = "root";
        for (auto it = trace_.rbegin(); it < trace_.rend(); ++it) {
            result += "[" + *it + "]";
        }
        return result;
    }

private:
    std::vector<std::string> trace_;
};

class TypeMismatchError : public SchemaError {
public:
    TypeMismatchError(const std::string& expected, const std::string& actual) noexcept
        : SchemaError(), expected_(expected), actual_(actual) {}
    TypeMismatchError(rapidjson::Type expected, const std::string& actual) noexcept
        : TypeMismatchError(get_type_string(expected), actual) {}
    TypeMismatchError(const std::string& expected, rapidjson::Type actual) noexcept
        : TypeMismatchError(expected, get_type_string(actual)) {}
    TypeMismatchError(rapidjson::Type expected, rapidjson::Type actual) noexcept
        : TypeMismatchError(get_type_string(expected), get_type_string(actual)) {}
    ~TypeMismatchError() noexcept override = default;

    std::string get_compact() const noexcept override {
        return "Type mismatch at '" + build_trace() + "' (expected: " + expected_ + ", actual: " + actual_ + ")";
    }
    std::string get_prettified() const noexcept override {
        return "Type mismatch:\n"
               "  expected: " + build_trace() + "\n" +
               "  which is: " + actual_ + "\n" +
               "     to be: " + expected_;
    }

    const char* what() const noexcept override {
        return "Type mismatch";
    }

private:
    std::string expected_;
    std::string actual_;
};

class MissingKeyError : public SchemaError {
public:
    MissingKeyError(const std::string& key) : SchemaError(), key_(key) {}
    ~MissingKeyError() override = default;

    std::string get_compact() const noexcept override {
        return "Key '" + key_ + "' missing at '" + build_trace() + "'";
    }
    std::string get_prettified() const noexcept override {
        return "Missing required key:\n"
               "     expected: " + build_trace() + "\n" +
               "  to have key: " + key_;
    }

    const char* what() const noexcept override {
        return "Missing required key";
    }

private:
    std::string key_;
};


} // namespace json_model

#endif // JSON_MODEL_INCLUDE_JSON_MODEL_ERROR_H
