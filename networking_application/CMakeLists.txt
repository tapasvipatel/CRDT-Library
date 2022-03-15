cmake_minimum_required(VERSION 3.10)

project(crdt-replica-application VERSION 1.0.0 LANGUAGES CXX)

link_directories(replica ${CMAKE_BINARY_DIR}/src)
add_executable(replica replica.cpp)

target_link_libraries(replica libcrdt.a -pthread)
