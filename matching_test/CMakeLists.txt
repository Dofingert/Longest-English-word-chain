cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

#FIND_PACKAGE(OpenMP REQUIRED)
#if(OPENMP_FOUND)
#    set(CMAKE_C_FLAGS "${OpenMP_C_FLAGS}")
#endif()

add_executable(matching_test matching_test.cpp
        ../include/xxhash.h ../include/core.h ../include/testlib.h
        ../testlib/random_gen.cpp ../testlib/testlib.cpp matching_test.cpp)

target_link_libraries(matching_test core)
