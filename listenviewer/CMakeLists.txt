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
target_compile_definitions(listenviewer PRIVATE XML_STATIC)

if (MINGW)
target_link_options(listenviewer PRIVATE -mwindows -static -fstack-protector)
endif()

if (MSVC)
    target_link_options(listenviewer PRIVATE /SUBSYSTEM:WINDOWS)
    target_compile_definitions(listenviewer PRIVATE _WIN32_WINNT=0x0A00)
endif()

target_link_libraries(listenviewer PRIVATE util)
target_link_libraries(listenviewer PRIVATE ${EXPAT_LIBRARIES})
target_link_libraries(listenviewer PRIVATE ${wxWidgets_LIBRARIES})
target_link_libraries(listenviewer PRIVATE ${Boost_LIBRARIES})

if (WIN32)
target_link_libraries(listenviewer PRIVATE bcrypt)
target_link_libraries(listenviewer PRIVATE ws2_32)
target_link_libraries(listenviewer PRIVATE mswsock)
endif()
