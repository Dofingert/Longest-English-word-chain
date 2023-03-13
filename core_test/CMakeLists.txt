cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

add_executable(core_test core_test.cpp
        ../include/xxhash.h ../include/core.h ../include/testlib.h
        ../testlib/random_gen.cpp ../testlib/testlib.cpp core_test.cpp)

target_link_libraries(core_test core gtest)
