cmake_minimum_required(VERSION 3.10)

project(crdt-client-application VERSION 1.0.0 LANGUAGES CXX)

link_directories(client ${CMAKE_BINARY_DIR}/src)
add_executable(client client.cpp)

target_link_libraries(client libcrdt.a)
