make_minimum_required(VERSION 3.10)

project(crdtlib VERSION 1.0.0 LANGUAGES CXX)

add_library(crdtlib SHARED
    CounterOB.cpp
    CrdtHandle.cpp
    CrdtObject.cpp
    Server.cpp
)

target_include_directories(crdtlib PRIVATE include)
