# json-model
[![Build Status](https://travis-ci.org/Forestryks/json-model.svg?branch=master)](https://travis-ci.org/Forestryks/json-model) [![codecov](https://codecov.io/gh/Forestryks/json-model/branch/master/graph/badge.svg)](https://codecov.io/gh/Forestryks/json-model)

## Introduction
Json-model is library providing nice way of JSON to C++ structs mappings.
 - Json-model is ___header-only___ and has no external dependencies except [rapidjson](https://github.com/Tencent/rapidjson), which is bundled.
 - Json-model is ___fast___ as it uses [rapidjson](https://github.com/Tencent/rapidjson) internally.
 - Json-model is ___error-safe___. In comparison with libraries that provide type adapters, with json-model you can't forget to put anything in JSON.
 - Json-model is ___simple___. Just create model, and use `.to_json()` and `.from_json()`. Json-model will handle the rest.
  
## Installation
Just copy `include/json_model` to project's include path and you are ready to go.

#### Running tests
If you want to run tests locally, do the following in repository root
```bash
mkdir build
cd build
cmake .. && cmake --build .
./test/unit_tests
```

## Usage

#### Example

This example shows how to create a model, read it from JSON, write to JSON, and handle errors.

```c++
#include <iostream>
#include "json_model/model.h"

struct LocationModel : public json_model::Model {
    DECLARE_FIELD(lon, double);
    DECLARE_FIELD(lat, double);

    PROVIDE_DETAILS(
        LocationModel,
        lon(_, "longitude"),
        lat(_, "latitude")
    );
};

struct PersonModel : public json_model::Model {
    DECLARE_FIELD(name, std::string);
    DECLARE_FIELD(surname, std::string);
    DECLARE_FIELD(gender, std::optional<std::string>);
    DECLARE_FIELD(location, std::variant<std::string, std::unique_ptr<LocationModel>>);
    DECLARE_FIELD(children, std::vector<std::string>);

    PROVIDE_DETAILS(
        PersonModel,
        name(_, "name"),
        surname(_, "surname"),
        gender(_, "gender"),
        location(_, "location"),
        children(_, "children")
    );
};

int main() {
    PersonModel model;

    std::string json_str = R"({
        "name": "John",
        "surname": "Smith",
        "location": {
            "longitude": -0.118092,
            "latitude": 51.509865
        },
        "children": ["Mary", "James"]
    })";

    try {
        model.from_json(json_str);
    } catch (Exception &exception) {
        std::cout << exception.get_prettified() << std::endl;
    }

    model.get_children().push_back("Alice");
    model.set_location("London");
    model.set_gender("male");

    std::cout << model.to_json() << std::endl;
}
```

#### Declaring a model
To declare a model declare a struct inherited from `json_model::Model`. Than add fields to it using `DECLARE_FIELD` and `PROVIDE_DETAILS` macros.

 1. Declare a struct inherited from `json_model::Model`.
 2. Add fields to model using `DECLARE_FIELD(field_name, field_type)` macro. Setter and getter will get defined too. The types that you can use are discussed below.
 3. Use `PROVIDE_DETAILS(class_name, ...)` macro to define constructor and `to_json()` and `from_json()` methods. First argument to this macro is class name, and other arguments are fields' descriptions in the following form: `field_name(_, "field_json_name")`, which is self-explanatory.

Check an example above for better understanding.

#### Supported field types
 - ___Primitives___: `bool`, `double`, `int`, `int64_t`, `unsigned`, `uint64_t`, `std::string` and `std::nullptr_t`
 - ___Pointers___: to use nested objects use `std::unique_ptr`, this is only allowed way of nesting. Pointer must be always not-null, for optional fields use `std::optional`
 - ___Containers___:
   - Use `std::vector` of _primitives_, _pointers_ or _containers_ for JSON arrays
   - Use `std::map` of _primitives_, _pointers_ or _containers_ for JSON objects
   - Use `std::variant` of _primitives_, _pointers_, _std::vector_ or _std::map_ for multiple allowed types for field
 - ___Optional___: all fields are by default required and emit error if not present while parsing JSON string. Use `std::optional` for optional fields

__Note on `std::variant`:__ when parsing JSON string to std::variant, json-model tries types in the order they appear in std::variant. To achieve better performance place the most common type first.

#### To and from JSON
 - Use `std::string json_model::Model::to_json()` to get JSON string from model. It will always succeed (if model doesn't contain anything in bad state __including empty `std::unique_ptr`__)
 - Use `bool json_model::Model::from_json(const std::string &json_str, bool throw_on_error = true)` to parse JSON string to model. On error `json_model::Exception` will be thrown or `false` returned if `throw_on_error == false`.

#### Error handling
Don't use `json_model::Exception::what()`, as it doesn't give any information about an error. Instead use `json_model::Exception::get_compact()` for compact error string, and `json_model::Exception::get_prettified()` for user-friendly __multiline__ error string. They provide usefull information as error position, reason and stack trace.

__Note on `std::variant`:__ If json-model fails to create variant from JSON string, it will use stack trace from the last type in std::variant for an error.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

##### Feel free to contribute
