# Copyright 2021 Ingemar Hedvall
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.20)
project(MdfViewer
        VERSION 0.1
        DESCRIPTION "Simple MDF file browser"
        LANGUAGES CXX)

add_executable(mdfview
        src/mdfviewer.cpp src/mdfviewer.h
        src/mainframe.cpp src/mainframe.h
        src/mdfdocument.cpp src/mdfdocument.h
        src/mdfview.cpp src/mdfview.h
        src/mdfviewer.rc src/childframe.cpp src/childframe.h src/windowid.h src/channelobserverlistview.cpp src/channelobserverlistview.h src/channelobserverframe.cpp src/channelobserverframe.h)


target_include_directories(mdfview PRIVATE ${wxWidgets_INCLUDE_DIRS})
target_include_directories(mdfview PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(mdfview PRIVATE "../../mdflib/include")
target_include_directories(mdfview PRIVATE "../../utillib/include")

target_link_options(mdfview PRIVATE -mwindows)

target_link_libraries(mdfview PRIVATE mdf)
target_link_libraries(mdfview PRIVATE util)
target_link_libraries(mdfview PRIVATE ${EXPAT_LIBRARIES})
target_link_libraries(mdfview PRIVATE ${wxWidgets_LIBRARIES})
target_link_libraries(mdfview PRIVATE ${Boost_LIBRARIES})
target_link_libraries(mdfview PRIVATE ${ZLIB_LIBRARIES})
target_link_libraries(mdfview PRIVATE ${OPENSSL_CRYPTO_LIBRARIES})
target_link_libraries(mdfview PRIVATE libiconv ${Iconv_LIBRARIES})
target_link_libraries(mdfview PRIVATE bcrypt.lib)

if (DOXYGEN_FOUND AND (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease") )
    set(DOXYGEN_RECURSIVE YES)
    doxygen_add_docs( doc_mdfviewer ALL
            src
            COMMENT "MDF VIEWER application generating API documentation with Doxygen")
endif()