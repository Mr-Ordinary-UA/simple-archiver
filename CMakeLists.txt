cmake_minimum_required(VERSION 3.15)
project(simple_archiver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(archive archive.cpp)

if(MSVC)
    set_property(TARGET archive PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()