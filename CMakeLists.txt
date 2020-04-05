cmake_minimum_required(VERSION 3.15)
project(cpp_threads)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS -pthread)

add_executable(cpp_threads main.cpp main.cpp)