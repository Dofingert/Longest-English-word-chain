cmake_minimum_required(VERSION 3.5)
project(core)

set(CMAKE_CXX_STANDARD 14)

FIND_PACKAGE(OpenMP REQUIRED)
message("FINDING OPENMP")
if(OPENMP_FOUND)
    message("OPENMP FOUND")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_library(core SHARED core.cpp ../../include/core.h)

target_compile_definitions(core PRIVATE EXPORT_FPIC)
