cmake_minimum_required(VERSION 3.10)

project(database-application VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
link_directories(database_application ${CMAKE_BINARY_DIR}/src)
add_executable(database_application main.cpp)

target_link_libraries(database_application libcrdt.a -pthread -ltgui -lsfml-graphics -lsfml-window -lsfml-system -lpq)

