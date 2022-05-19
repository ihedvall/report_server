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
target_compile_definitions(listend PRIVATE XML_STATIC)

if (MINGW)
target_link_options(listend PRIVATE -static)
endif()

if (MSVC)
    target_compile_definitions(listend PRIVATE _WIN32_WINNT=0x0A00)
endif()

target_link_libraries(listend PRIVATE util)
target_link_libraries(listend PRIVATE ${EXPAT_LIBRARIES})
target_link_libraries(listend PRIVATE ${Boost_LIBRARIES})

if (WIN32)
target_link_libraries(listend PRIVATE bcrypt)
target_link_libraries(listend PRIVATE ws2_32)
target_link_libraries(listend PRIVATE mswsock)
endif()