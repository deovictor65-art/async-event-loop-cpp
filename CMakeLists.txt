cmake_minimum_required(VERSION 3.20)
project(async_event_loop VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

if (MSVC)
    add_compile_options(/W4 /O2 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -O3 -march=native)
endif()

add_library(netloop_core INTERFACE)
target_include_directories(netloop_core INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_executable(netloop_bench src/main.cpp)
target_link_libraries(netloop_bench PRIVATE netloop_core Threads::Threads)

enable_testing()
add_executable(netloop_tests tests/test_netloop.cpp)
target_link_libraries(netloop_tests PRIVATE netloop_core Threads::Threads)

add_test(NAME EventLoopTests COMMAND netloop_tests)
