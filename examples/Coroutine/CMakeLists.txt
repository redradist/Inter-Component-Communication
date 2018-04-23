cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_COMPILER "/usr/local/bin/clang" )
set(CMAKE_CXX_COMPILER "/usr/local/bin/clang++")
add_definitions(-D__STRICT_ANSI__)

project(CoroutineExample)

set(CMAKE_CXX_FLAGS "-fcoroutines-ts -stdlib=libc++")

include_directories(../../src)
include_directories(/usr/local/lib/boost/include)
link_directories(/usr/local/lib/boost/lib)

link_directories(/usr/local/lib)
add_subdirectory(../../ ${CMAKE_BINARY_DIR}/bin)
add_executable(CoroutineExample main.cpp)
target_link_libraries(CoroutineExample ICC pthread boost_system)