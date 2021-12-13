# Copyright 2021 Ingemar Hedvall
# SPDX-License-Identifier: MIT

project(TestMdf
        VERSION 1.0
        DESCRIPTION "Google unit tests for the mdf library"
        LANGUAGES CXX)

enable_testing()

add_executable(test_mdf test_conversion.cpp testread.cpp testread.h testwrite.cpp testwrite.h)

target_include_directories(test_mdf PRIVATE
        ../include
        ../src
        ../../utillib/include
        ${GTEST_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS})

target_link_libraries(test_mdf PRIVATE  mdf)
target_link_libraries(test_mdf PRIVATE util)
target_link_libraries(test_mdf PRIVATE ${EXPAT_LIBRARIES})
target_link_libraries(test_mdf PRIVATE ${Boost_LIBRARIES})
target_link_libraries(test_mdf PRIVATE ${ZLIB_LIBRARIES})
target_link_libraries(test_mdf PRIVATE ${OPENSSL_CRYPTO_LIBRARIES})
target_link_libraries(test_mdf PRIVATE ${GTEST_BOTH_LIBRARIES})
target_link_libraries(test_mdf PRIVATE libiconv)

include(GoogleTest)
gtest_discover_tests(test_mdf)

