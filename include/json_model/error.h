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
private:
    std::string segment_;
    size_t offset_;
    size_t local_offset_;
    std::string reason_;

    const inline static size_t SEGMENT_SIZE = 30;
};

} // namespace json_model

#endif // JSON_MODEL_INCLUDE_JSON_MODEL_ERROR_H
