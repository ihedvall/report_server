# Copyright 2021 Ingemar Hedvall
# SPDX-License-Identifier: MIT

project(TestUtil
        VERSION 1.0
        DESCRIPTION "Google unit tests for the util library"
        LANGUAGES CXX)

add_executable(test_util
        test_logfile.cpp
        test_timestamp.cpp
        test_logging.cpp
        test_xml.cpp testzlib.cpp testzlib.h testcrypto.cpp teststringutil.cpp)

target_include_directories(test_util PRIVATE
        ../include
        ../src
        ${GTEST_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS})

target_link_libraries(test_util PRIVATE util)
target_link_libraries(test_util PRIVATE ${EXPAT_LIBRARIES})
target_link_libraries(test_util PRIVATE ${ZLIB_LIBRARIES})
target_link_libraries(test_util PRIVATE ${OPENSSL_CRYPTO_LIBRARIES})
target_link_libraries(test_util PRIVATE ${GTEST_BOTH_LIBRARIES})


add_test(TestUtil test_util)

