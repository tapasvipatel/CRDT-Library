cmake_minimum_required(VERSION 3.10)

project(crdt-test-application VERSION 1.0.0 LANGUAGES CXX)

link_directories(test_application ${CMAKE_BINARY_DIR}/src)
add_executable(test_application test_application.cpp)

target_link_libraries(test_application libcrdt.a)
