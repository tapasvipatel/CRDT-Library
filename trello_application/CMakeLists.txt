cmake_minimum_required(VERSION 3.10)

project(trello-application VERSION 1.0.0 LANGUAGES CXX)

link_directories(trello_application ${CMAKE_BINARY_DIR}/src)
add_executable(trello_application main.cpp)

target_link_libraries(trello_application libcrdt.a -pthread -ltgui -lsfml-graphics -lsfml-window -lsfml-system)
