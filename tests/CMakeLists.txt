cmake_minimum_required(VERSION 3.10)

project(crdt-test VERSION 1.0.0 LANGUAGES CXX)

add_compile_definitions(BUILD_TESTING=1)

link_directories(crdt_test ${CMAKE_BINARY_DIR}/src)
add_executable(crdt_test test.cpp)

find_package(Catch2)
target_link_libraries(crdt_test libcrdt.a Catch2::Catch2WithMain)

