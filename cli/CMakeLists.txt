cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 14)

add_executable(cli cli_main.cpp cli_main.h ../include/core.h)
add_executable(static_cli cli_main.cpp cli_main.h ../core/core.cpp ../include/core)

target_compile_definitions(static_cli PRIVATE STATIC_LINK)
target_link_libraries(cli core)
