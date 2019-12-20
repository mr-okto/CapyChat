cmake_minimum_required(VERSION 3.15)
project(capy-chat)

set(CMAKE_CXX_STANDARD 17)

find_package(GnuTLS REQUIRED)
find_package(opendht REQUIRED)

include_directories(/usr/local/include ./include ./bundled-tools)
link_directories(/usr/local/lib)

add_executable(bootstrap_node
        src/node.cpp
        src/utils.cpp
        src/dht_config.cpp
        )

target_link_libraries(bootstrap_node opendht gnutls)
