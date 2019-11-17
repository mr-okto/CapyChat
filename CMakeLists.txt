cmake_minimum_required(VERSION 3.15)
project(capy-chat)

set(CMAKE_CXX_STANDARD 17)

find_package(GnuTLS REQUIRED)
find_package(opendht REQUIRED)

include_directories(/usr/local/include ./include)
link_directories(/usr/local/lib)

add_executable(main.exe src/main.cpp)
target_link_libraries(main.exe opendht gnutls)

add_subdirectory(bundled-tools)
