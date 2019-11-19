cmake_minimum_required(VERSION 3.15)
project(capy-chat)

set(CMAKE_CXX_STANDARD 17)

find_package(GnuTLS REQUIRED)
find_package(opendht REQUIRED)

include_directories(/usr/local/include ./include)
link_directories(/usr/local/lib)

add_executable(host_node src/host_node.cpp src/utils.cpp)
target_link_libraries(host_node opendht gnutls)

add_executable(client_node src/client_node.cpp src/utils.cpp)
target_link_libraries(client_node opendht gnutls)

add_subdirectory(bundled-tools)
