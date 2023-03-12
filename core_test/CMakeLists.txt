cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

add_executable(core_test core_test.cpp xxhash.h ../include/core.h random_gen.cpp)

target_link_libraries(core_test core gtest)
