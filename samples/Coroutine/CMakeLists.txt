cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER "/usr/bin/clang-10" )
set(CMAKE_CXX_COMPILER "/usr/bin/clang++-10")
add_definitions(-D__STRICT_ANSI__)

project(CoroutineExample)

set(CMAKE_CXX_FLAGS "-fcoroutines-ts -stdlib=libc++")

include_directories(../../src)
include_directories(/usr/local/boost_1_69_0/include)
link_directories(/usr/local/boost_1_69_0/lib)

link_directories(/usr/local/lib)
add_subdirectory(../../ ${CMAKE_BINARY_DIR}/bin)
add_executable(CoroutineExample main.cpp)
target_link_libraries(CoroutineExample ICC pthread boost_system)