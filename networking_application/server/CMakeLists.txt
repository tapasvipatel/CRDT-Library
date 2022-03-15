cmake_minimum_required(VERSION 3.10)

project(crdt-server-application VERSION 1.0.0 LANGUAGES CXX)

link_directories(server ${CMAKE_BINARY_DIR}/src)
add_executable(server server.cpp)

target_link_libraries(server libcrdt.a)
