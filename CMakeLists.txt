cmake_minimum_required(VERSION 3.13)
project(capy-chat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_compile_options(-fPIC)

find_package(GnuTLS REQUIRED)
find_package(opendht REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets Network Gui REQUIRED)

include_directories(/usr/local/include project/include)
link_directories(/usr/local/lib project/src)

add_executable(chat
        project/chatwindow.ui
        project/src/chatclient.cpp
        project/src/chatwindow.cpp
        project/src/clientmain.cpp
        project/src/utils.cpp
        project/src/node_tools.cpp
        project/src/popup.cpp)

target_link_libraries(chat libc.a Qt5::Widgets Qt5::Network Qt5::Core Qt5::Gui readline opendht gnutls libstdc++fs.a libstdc++.a libgcc.a libpthread.a)
