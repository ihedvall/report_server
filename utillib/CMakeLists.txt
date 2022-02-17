# Copyright 2021 Ingemar Hedvall
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.20)
project(UtilLib
        VERSION 1.0
        DESCRIPTION "Library for common supports as log files"
        LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_DEBUG_POSTFIX d)

include("../script/boost.cmake")
include("../script/zlib.cmake")
include("../script/openssl.cmake")
include("../script/expat.cmake")
include("../script/sqlite3.cmake")
include("../script/googletest.cmake")
include("../script/doxygen.cmake")

add_library(util STATIC
        src/logfile.cpp src/logfile.h
        include/util/timestamp.h src/timestamp.cpp
        include/util/logconfig.h src/logconfig.cpp
        include/util/logging.h src/logging.cpp
        src/logconsole.cpp src/logconsole.h
        include/util/logmessage.h src/logmessage.cpp
        include/util/logstream.h src/logstream.cpp
        src/expatxml.h src/expatxml.cpp
        src/xmlnode.h src/xmlnode.cpp
        include/util/ixmlnode.h src/ixmlnode.cpp
        include/util/ixmlfile.h src/ixmlfile.cpp
        include/util/stringutil.h src/stringutil.cpp
        include/util/ilogger.h
        include/util/zlibutil.h src/zlibutil.cpp
        include/util/cryptoutil.h src/cryptoutil.cpp
        include/util/csvwriter.h src/csvwriter.cpp src/messagequeue.cpp src/messagequeue.h src/listenmessage.h src/ilisten.cpp include/util/ilisten.h src/listenserver.cpp src/listenserver.h src/listenproxy.cpp src/listenproxy.h src/listenmessage.cpp include/util/threadsafequeue.h src/listenserverconnection.h src/listenserverconnection.cpp src/listenconfig.cpp src/listenconfig.h src/listenclient.cpp src/listenclient.h)

target_include_directories(util PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )

target_include_directories(util PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(util PRIVATE ${EXPAT_INCLUDE_DIRS})
target_include_directories(util PRIVATE ${ZLIB_INCLUDE_DIRS})
target_include_directories(util PRIVATE ${OPENSSL_INCLUDE_DIR})

set(UTIL_PUBLIC_HEADERS
    include/util/cryptoutil.h
    include/util/csvwriter.h
    include/util/ilogger.h
    include/util/ixmlnode.h
    include/util/logconfig.h
    include/util/logging.h
    include/util/logmessage.h
    include/util/stringutil.h
    include/util/timestamp.h
    include/util/zlibutil.h
)

set_target_properties(util PROPERTIES PUBLIC_HEADER "${UTIL_PUBLIC_HEADERS}")
get_target_property(PH util PUBLIC_HEADER)
message( STATUS "UTIL Target Includes: " "${PH}")

if (GTEST_FOUND)
    enable_testing()
    add_subdirectory(test)
endif()

if (DOXYGEN_FOUND AND (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease") )
    set(DOXYGEN_RECURSIVE YES)
    doxygen_add_docs( doc_libutil ALL
            include/util
            COMMENT "Util Library generating API documentation with Doxygen")
endif()

install(TARGETS util
        EXPORT UtilLibTargets
        LIBRARY DESTINATION util/lib
        ARCHIVE DESTINATION util/lib
        RUNTIME DESTINATION util/bin
        PUBLIC_HEADER DESTINATION util/include/util)

#include(CMakePackageConfigHelpers)
#write_basic_package_version_file(
#        UtilLibConfigVersion.cmake
#        VERSION ${PACKAGE_VERSION}
#        COMPATIBILITY AnyNewerVersion)
#
#install(EXPORT UtilLibTargets
#        FILE UtilLibTargets.cmake
#        NAMESPACE UtilLib::
#        DESTINATION lib/cmake/UtilLib
#        )

#export(TARGETS util NAMESPACE UtilLib:: FILE UtilLibTargets.cmake)