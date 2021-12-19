cmake_minimum_required(VERSION 3.10)

project(crdt VERSION 1.0.0 LANGUAGES CXX)

add_library(crdt SHARED
    CrdtHandle.cpp
    CrdtObject.cpp
    Server.cpp
    operation_based/CounterOB.cpp
    state_based/PNCounterSB.cpp
)

#target_include_directories(crdt PRIVATE include)
