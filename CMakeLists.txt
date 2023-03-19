cmake_minimum_required(VERSION 3.5)
project(longest-english-word-chain)

set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-O0")
set(CMAKE_CXX_STANDARD 14)

# For Google test
set(GOOGLETEST_VERSION 1.13.0)
add_subdirectory(test/googletest)
include_directories(test/googletest/include)
link_directories(test/googletest)

include_directories(./include src/parallel_hashmap)
add_subdirectory(src/core)
#add_subdirectory(core_other)
add_subdirectory(src/cli)
add_subdirectory(test)
add_subdirectory(src/matching_test)
