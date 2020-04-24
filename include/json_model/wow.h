//
// Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
//

#ifndef JSON_MODEL_INCLUDE_JSON_MODEL_WOW_H
#define JSON_MODEL_INCLUDE_JSON_MODEL_WOW_H

#include <string>

inline std::string f(int x) {
    if (x == 0) {
        return "hello";
    } else if (x == 1) {
        return "world";
    } else {
        throw 123;
    }
}

inline std::string g() {
    return f(0) + " " + f(1);
}

#endif // JSON_MODEL_INCLUDE_JSON_MODEL_WOW_H
