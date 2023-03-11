cmake_minimum_required(VERSION 3.5)
project(core)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory("core")
#add_executable(core main.cpp)
add_executable(cli cli/cli_main.cpp cli/cli_main.h)

target_link_libraries(cli core)
# For Google test
#set(GOOGLETEST_VERSION 1.13.0)
#add_subdirectory(./googletest)
#include_directories(${PROJECT_SOURCE_DIR}/include ./googletest/include)
#link_directories(${PROJECT_SOURCE_DIR}/lib ${PROJECT_SOURCE_DIR}/googletest)
#target_link_libraries(${PROJECT_NAME} gtest)
