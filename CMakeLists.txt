cmake_minimum_required(VERSION 3.5)
project(longest-english-word-chain)

set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-O0")
set(CMAKE_CXX_STANDARD 14)

# For Google test
set(GOOGLETEST_VERSION 1.13.0)
add_subdirectory(./googletest)
include_directories(./googletest/include ./include)
link_directories(./googletest)

add_subdirectory(core)
#add_subdirectory(core_other)
add_subdirectory(cli)
add_subdirectory(core_test)
add_subdirectory(matching_test)
