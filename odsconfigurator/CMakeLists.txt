# Copyright 2022 Ingemar Hedvall
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.20)
project(OdsConfigurator
        VERSION 1.0
        DESCRIPTION "ODS Database Configurator"
        LANGUAGES CXX)

add_executable(odsconfig 
        src/odsconfig.cpp src/odsconfig.h
        src/mainframe.cpp src/mainframe.h
        src/odsdocument.cpp src/odsdocument.h
        src/odsview.cpp src/odsview.h
        src/odsconfig.rc
        src/childframe.cpp src/childframe.h
        src/odsconfigid.h src/commonpanel.cpp src/commonpanel.h src/enumpanel.cpp src/enumpanel.h src/tablepanel.cpp src/tablepanel.h)


target_include_directories(odsconfig PRIVATE ${wxWidgets_INCLUDE_DIRS})
target_include_directories(odsconfig  PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(odsconfig PRIVATE "../../mdflib/include")
target_include_directories(odsconfig PRIVATE "../../utillib/include")

target_link_options(odsconfig PRIVATE -mwindows -static )

target_link_libraries(odsconfig PRIVATE ods)
target_link_libraries(odsconfig PRIVATE util)
target_link_libraries(odsconfig PRIVATE ${EXPAT_LIBRARIES})
target_link_libraries(odsconfig PRIVATE ${wxWidgets_LIBRARIES})
target_link_libraries(odsconfig PRIVATE ${Boost_LIBRARIES})
target_link_libraries(odsconfig PRIVATE ${ZLIB_LIBRARIES})
target_link_libraries(odsconfig PRIVATE crypto ) 
target_link_libraries(odsconfig PRIVATE iconv )
target_link_libraries(odsconfig PRIVATE bcrypt)
target_link_libraries(odsconfig PRIVATE ws2_32)
#target_link_libraries(odsconfig PRIVATE wsock32)

if (DOXYGEN_FOUND AND (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease") )
    set(DOXYGEN_RECURSIVE YES)
    doxygen_add_docs( doc_odsconfig ALL
            src
            COMMENT "ODS CONFIGURATOR application generating ODS database models")
endif()