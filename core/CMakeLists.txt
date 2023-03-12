cmake_minimum_required(VERSION 3.5)
project(core)

set(CMAKE_CXX_STANDARD 14)

add_library(core SHARED core.cpp ../include/core.h)

target_compile_definitions(core PRIVATE EXPORT_FPIC)
