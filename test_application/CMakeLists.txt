cmake_minimum_required(VERSION 3.10)

project(crdt-test VERSION 1.0.0 LANGUAGES CXX)

link_directories(crdt_test_application ${CMAKE_BINARY_DIR}/src)
add_executable(crdt_test_application test_application.cpp)

target_link_libraries(crdt_test_application libcrdt.a -lboost_system -pthread)
