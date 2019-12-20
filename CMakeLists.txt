cmake_minimum_required(VERSION 3.13)
project(capy-chat)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(GnuTLS REQUIRED)
find_package(opendht REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets Network Gui REQUIRED)

include_directories(/usr/local/include ./include ./bundled-tools)
link_directories(/usr/local/lib)

add_executable(chat
        project/chatwindow.ui
        project/chatclient.cpp
        project/chatwindow.cpp
        project/clientmain.cpp
        project/utils.cpp
        project/node_tools.cpp
        )

target_link_libraries(chat Qt5::Widgets Qt5::Network Qt5::Core Qt5::Gui readline opendht-static gnutls stdc++fs)
