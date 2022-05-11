# Copyright 2022 Ingemar Hedvall
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.20)
project(serviced
        VERSION 1.0
        DESCRIPTION "Service runs applications"
        LANGUAGES CXX)

add_executable(serviced src/serviced.cpp src/servicehelper.cpp src/servicehelper.h)



target_include_directories(serviced PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(serviced PRIVATE "../../utillib/include")

target_link_options(serviced PRIVATE -static)

target_link_libraries(serviced PRIVATE util)
target_link_libraries(serviced PRIVATE ${EXPAT_LIBRARIES})
target_link_libraries(serviced PRIVATE ${Boost_LIBRARIES})
target_link_libraries(serviced PRIVATE ${ZLIB_LIBRARIES})
target_link_libraries(serviced PRIVATE ${OPENSSL_CRYPTO_LIBRARY})
target_link_libraries(serviced PRIVATE iconv)
target_link_libraries(serviced PRIVATE bcrypt)
target_link_libraries(serviced PRIVATE ws2_32)
target_link_libraries(serviced PRIVATE mswsock)

if (DOXYGEN_FOUND AND (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease") )
    set(DOXYGEN_RECURSIVE YES)
    doxygen_add_docs( doc_services ALL
            src
            COMMENT "Service application Doxygen")
endif()