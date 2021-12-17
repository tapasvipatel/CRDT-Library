make_minimum_required(VERSION 3.10)

project(CRDT LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "")
set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_CXX_FLAGS_RELEASE "")

add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
