cmake_minimum_required(VERSION 3.10)
project(TemperatureLogger)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(device_simulator device_simulator.cpp)
add_executable(temperature_logger temperature_logger.cpp)