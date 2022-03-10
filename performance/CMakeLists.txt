cmake_minimum_required(VERSION 3.10)

project(crdt-performance VERSION 1.0.0 LANGUAGES CXX)

link_directories(performance ${CMAKE_BINARY_DIR}/src)
add_executable(performance performance.cpp)

target_link_libraries(performance libcrdt.a)
