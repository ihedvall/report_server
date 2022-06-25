# Copyright 2021 Ingemar Hedvall
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.20)

project(ReportServer
        VERSION 0.9
        DESCRIPTION "Application with ODS, MDF and HDF technology dependencies"
        LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Static libraries are preferred" OFF)
option(BUILD_STATIC_LIBS "Static libraries are normally used" ON)
option(BUILD_DOC "If doxygen is installed, then build documentation in Release mode" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_DEBUG_POSTFIX d)

include(CMakePrintHelpers)

# The following external libraries are required
include("script/boost.cmake")
include("script/zlib.cmake")
include("script/openssl.cmake")
include("script/expat.cmake")

include("script/utillib.cmake")
include("script/mdflib.cmake")
include("script/odslib.cmake")
include("script/wxwidgets.cmake")
# include("script/iconv.cmake")

if (MSVC)
    include("script/protobuf.cmake")
    include("script/grpc.cmake")
endif()


if (wxWidgets_FOUND )
add_subdirectory(reportexplorer)
endif()





