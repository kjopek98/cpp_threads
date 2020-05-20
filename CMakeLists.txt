cmake_minimum_required(VERSION 3.15)
project(cpp_threads)

set(CMAKE_CXX_STANDARD 17)

add_executable(cpp_threads
        src/States.h
        src/ThreadPool.cpp
        src/ThreadPool.h
        src/Player.cpp
        src/Player.h
        src/GameSession.cpp
        src/GameSession.h
        src/RequestData.h
        src/Server.cpp
        src/Server.h
        src/main.cpp
        )