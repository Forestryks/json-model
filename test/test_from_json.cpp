//
// Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
//

#include <json_model/model.h>

#include <gtest/gtest.h>

////////////////////////////////////////////////////////////////////////////////

struct SimpleModel : public json_model::Model {
    DECLARE_FIELD(string, std::string);

    PROVIDE_DETAILS(
        SimpleModel,
        string(_, "string")
    )
};

TEST(from_json, simple) {
    SimpleModel model;
    ASSERT_THROW(
        model.from_json(R"({"bad_json":}})"),
        json_model::ParseError
    );
}

////////////////////////////////////////////////////////////////////////////////
