cmake_minimum_required(VERSION 3.10)

project(crdt-test VERSION 1.0.0 LANGUAGES CXX)

link_directories(crdt_test ${CMAKE_BINARY_DIR}/src)
add_executable(crdt_test main.cpp)
target_link_libraries(crdt_test libcrdt.a)
