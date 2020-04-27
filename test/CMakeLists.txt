add_executable(
    unit_tests
    test_error.cpp
    test_traits.cpp
    test_to_json.cpp
)

target_link_libraries(
    unit_tests PRIVATE
    gtest_main
)

target_compile_options(
    unit_tests PRIVATE
    -O0 -g --coverage -fno-inline -fno-inline-small-functions -fno-default-inline
)

if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(unit_tests PRIVATE --coverage)
else()
    target_link_libraries(unit_tests PRIVATE --coverage)
endif()

add_test(
    NAME unit_tests
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/unit_tests
)
