# Copyright 2022 Ingemar Hedvall
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.20)
project(listend
        VERSION 1.0
        DESCRIPTION "Daemon  that run all listen servers"
        LANGUAGES CXX)

add_executable(listend src/listend.cpp)

target_include_directories(listend PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(listend PRIVATE "../../utillib/include")

target_link_options(listend PRIVATE -static)

target_link_libraries(listend PRIVATE util)
target_link_libraries(listend PRIVATE ${EXPAT_LIBRARIES})
target_link_libraries(listend PRIVATE ${Boost_LIBRARIES})
target_link_libraries(listend PRIVATE ${ZLIB_LIBRARIES})
target_link_libraries(listend PRIVATE crypto)
target_link_libraries(listend PRIVATE iconv )
target_link_libraries(listend PRIVATE bcrypt)
target_link_libraries(listend PRIVATE ws2_32)
target_link_libraries(listend PRIVATE mswsock)

if (DOXYGEN_FOUND AND (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease") )
    set(DOXYGEN_RECURSIVE YES)
    doxygen_add_docs( doc_listend ALL
            src
            COMMENT "Listend Doxygen")
endif()