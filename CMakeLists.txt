cmake_minimum_required(VERSION 3.15)
project(capy-chat)

set(CMAKE_CXX_STANDARD 17)

find_package(GnuTLS REQUIRED)
find_package(opendht REQUIRED)

include_directories(/usr/local/include ./include ./bundled-tools)
link_directories(/usr/local/lib)

add_executable(node
        src/node.cpp
        src/utils.cpp
        src/bundled_tools.cpp
        )

target_link_libraries(node readline opendht gnutls)

add_subdirectory(bundled-tools)
