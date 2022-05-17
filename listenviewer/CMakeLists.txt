# Copyright 2022 Ingemar Hedvall
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.20)
project(ListenViewer
        VERSION 1.0
        DESCRIPTION "Listen Window"
        LANGUAGES CXX)

add_executable(listenviewer
        src/listenviewer.cpp src/listenviewer.h
        src/mainframe.cpp src/mainframe.h
        src/listenviewer.rc
        src/listenviewerid.h
        src/settingdialog.cpp src/settingdialog.h
        src/listendialog.cpp src/listendialog.h src/listenlistview.cpp src/listenlistview.h)


target_include_directories(listenviewer PRIVATE ${wxWidgets_INCLUDE_DIRS})
target_include_directories(listenviewer PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(listenviewer PRIVATE "../../utillib/include")


target_link_options(listenviewer PRIVATE -mwindows -static -fstack-protector)

target_link_libraries(listenviewer PRIVATE util)
target_link_libraries(listenviewer PRIVATE ${EXPAT_LIBRARIES})
target_link_libraries(listenviewer PRIVATE ${wxWidgets_LIBRARIES})
target_link_libraries(listenviewer PRIVATE ${Boost_LIBRARIES})
target_link_libraries(listenviewer PRIVATE ${ZLIB_LIBRARIES})
#target_link_libraries(listenviewer PRIVATE ${OPENSSL_CRYPTO_LIBRARY})
target_link_libraries(listenviewer PRIVATE iconv)
target_link_libraries(listenviewer PRIVATE bcrypt)
target_link_libraries(listenviewer PRIVATE ws2_32)
target_link_libraries(listenviewer PRIVATE mswsock)

