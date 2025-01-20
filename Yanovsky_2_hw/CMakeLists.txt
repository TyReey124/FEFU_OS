
cmake_minimum_required(VERSION 3.10)
project(ProcessLauncher)

set(CMAKE_CXX_STANDARD 17)

add_library(process_launcher process_launcher.cpp)
add_executable(test_process_launcher test_process_launcher.cpp)
target_link_libraries(test_process_launcher process_launcher)
