cmake_minimum_required(VERSION 3.14)
project(windows_tets)

set(CMAKE_CXX_STANDARD 11)

add_executable(windows_tets main.cpp second_task.cpp)
add_executable(second_task second_task.cpp)
add_executable(main main.cpp)