cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

add_executable(core_test core_test.cpp
        ../include/xxhash.h ../include/core.h ../include/testlib.h
        ../src/publib/random_gen.cpp ../src/publib/testlib.cpp core_test.cpp
        ../src/core/core.cpp)

target_compile_definitions(core_test PRIVATE STATIC_LINK)
target_link_libraries(core_test gtest)
