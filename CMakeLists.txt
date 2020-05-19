cmake_minimum_required(VERSION 3.15)
project(cpp_threads)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS -pthread )

add_executable(cpp_threads
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
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
target_link_libraries(cpp_threads ${CURSES_LIBRARIES})