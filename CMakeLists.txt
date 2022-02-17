# Copyright 2021 Ingemar Hedvall
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.20)

project(ReportServer
        VERSION 0.2
        DESCRIPTION "Application with ODS, MDF and HDF technology dependencies"
        LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Static libraries are preferred" OFF)
option(BUILD_STATIC_LIBS "Static libraries are normally used" ON)
option(BUILD_DOC "If doxygen is installed, then build documentation in Release mode" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_DEBUG_POSTFIX d)

# The following external libraries are required
include("script/boost.cmake")
include("script/zlib.cmake")
include("script/openssl.cmake")
include("script/expat.cmake")

# wxWidgets and iconv is only required to build mdfviewer
include("script/wxwidgets.cmake")
include("script/iconv.cmake")

# doxygen is only used to build documentation
include("script/doxygen.cmake")

# googletest is only required to build unit tests
include("script/googletest.cmake")

add_subdirectory(utillib)
add_subdirectory(mdflib)
add_subdirectory(odslib)

if (wxWidgets_FOUND AND WIN32)
    add_subdirectory(mdfviewer)
    add_subdirectory(serviceexplorer)
endif()

if (WIN32)
    add_subdirectory(serviced)

endif()