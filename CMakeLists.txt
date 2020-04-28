#
# Copyright (c) 2020 Andrei Odintsov <forestryks1@gmail.com>
#

cmake_minimum_required(VERSION 3.12)
project(json-model)

set(CMAKE_CXX_STANDARD 17)

include(cmake/googletest.cmake)
fetch_googletest(
    ${PROJECT_SOURCE_DIR}/cmake
    ${PROJECT_BINARY_DIR}/googletest
)

set(
    CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-comment -Wcast-align -Wcast-qual -Wdisabled-optimization -Winit-self -Wunused -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=2 -Wimplicit-fallthrough -Werror"
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(
        CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -Wlogical-op -Wnoexcept -Wstrict-null-sentinel"
    )
endif ()

include_directories(include)
enable_testing()
add_subdirectory(test)
