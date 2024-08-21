cmake_minimum_required(VERSION 3.10)

project(connect4 VERSION 0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-g -Wall")

include_directories(include)
add_executable(main.exe src/main.cpp src/bot.cpp)
