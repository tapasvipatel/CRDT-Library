cmake_minimum_required(VERSION 3.10)

project(crdt-networking-application VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "")
set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_CXX_FLAGS_RELEASE "")

add_subdirectory(server)
add_subdirectory(client)
