# Copyright 2021 Ingemar Hedvall
# SPDX-License-Identifier: MIT

project(TestOdsl
        VERSION 1.0
        DESCRIPTION "Google unit tests for the ODS library"
        LANGUAGES CXX)

add_executable(test_ods
        testsqlite.h testsqlite.cpp)

target_include_directories(test_ods PRIVATE
        ../include
        ../src
        ../../utillib/include
        ${GTEST_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS})

target_link_libraries(test_ods PRIVATE ods)
target_link_libraries(test_ods PRIVATE util)
target_link_libraries(test_ods PRIVATE ${SQLite3_LIBRARIES})
target_link_libraries(test_ods PRIVATE ${EXPAT_LIBRARIES})
target_link_libraries(test_ods PRIVATE ${ZLIB_LIBRARIES})
target_link_libraries(test_ods PRIVATE ${OPENSSL_CRYPTO_LIBRARIES})
target_link_libraries(test_ods PRIVATE ${GTEST_BOTH_LIBRARIES})


include(GoogleTest)
gtest_discover_tests(test_ods)

