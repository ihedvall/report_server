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
        test_xml.cpp
        testzlib.cpp testzlib.h
        testcrypto.cpp
        teststringutil.cpp
        test_message_queue.cpp
        testlisten.cpp testlisten.h test_tempdir.cpp)

target_include_directories(test_util PRIVATE
        ../include
        ../src
        ${GTEST_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS})



target_link_libraries(test_util PRIVATE util)
target_link_libraries(test_util PRIVATE ${Boost_LIBRARIES})

target_link_libraries(test_util PRIVATE ${EXPAT_LIBRARIES})
target_link_libraries(test_util PRIVATE ${ZLIB_LIBRARIES})
target_link_libraries(test_util PRIVATE ${GTEST_BOTH_LIBRARIES})
target_link_libraries(test_util PRIVATE ${OPENSSL_CRYPTO_LIBRARY})
if (WIN32)
target_link_libraries(test_util PRIVATE ws2_32)
target_link_libraries(test_util PRIVATE mswsock)
target_link_libraries(test_util PRIVATE bcrypt)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(test_util PRIVATE -static -fstack-protector )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(test_util PRIVATE -D_WIN32_WINNT=0x0A00)
endif()


add_test(TestUtil test_util)

