cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 14)

add_executable(cli cli_main.cpp cli_main.h)

target_link_libraries(cli core)