cmake_minimum_required(VERSION 3.15)
project(capy-chat)

set(CMAKE_CXX_STANDARD 17)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

find_package(gnutls REQUIRED)
find_package(opendht REQUIRED)

add_executable(main.exe main.cpp)
target_link_libraries(main.exe opendht gnutls)

add_subdirectory(bundled-tools)
