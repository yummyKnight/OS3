cmake_minimum_required(VERSION 3.14)
project(windows_tets)

set(CMAKE_CXX_STANDARD 11)
add_executable(windows_tets main.cpp)
add_executable(second_task second_task.cpp)
#target_link_libraries(second_task libwinmm)
add_executable(main main.cpp)
add_executable(example example.cpp)