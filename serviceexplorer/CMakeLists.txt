# Copyright 2022 Ingemar Hedvall
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.20)
project(ServiceExplorer
        VERSION 1.0
        DESCRIPTION "Service Explorer for viewing Windows services"
        LANGUAGES CXX)

add_executable(serviceexplorer
        src/serviceexplorer.cpp src/serviceexplorer.h
        src/mainframe.cpp src/mainframe.h
        src/serviceexplorer.rc
        src/serviceexplorerid.h
        src/serviceinfo.cpp src/serviceinfo.h
        src/servicedialog.cpp src/servicedialog.h src/servicehelper.cpp src/servicehelper.h)


target_include_directories(serviceexplorer PRIVATE ${wxWidgets_INCLUDE_DIRS})
target_include_directories(serviceexplorer PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(serviceexplorer PRIVATE "../../utillib/include")

target_link_options(serviceexplorer PRIVATE -mwindows)

target_link_libraries(serviceexplorer PRIVATE util)
target_link_libraries(serviceexplorer PRIVATE ${EXPAT_LIBRARIES})
target_link_libraries(serviceexplorer PRIVATE ${wxWidgets_LIBRARIES})
target_link_libraries(serviceexplorer PRIVATE ${Boost_LIBRARIES})
target_link_libraries(serviceexplorer PRIVATE ${ZLIB_LIBRARIES})
target_link_libraries(serviceexplorer PRIVATE ${OPENSSL_CRYPTO_LIBRARIES})
target_link_libraries(serviceexplorer PRIVATE libiconv ${Iconv_LIBRARIES})
target_link_libraries(serviceexplorer PRIVATE bcrypt.lib)

if (DOXYGEN_FOUND AND (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease") )
    set(DOXYGEN_RECURSIVE YES)
    doxygen_add_docs( doc_serviceexplorer ALL
            src
            COMMENT "Service Explorer Documentation")
endif()