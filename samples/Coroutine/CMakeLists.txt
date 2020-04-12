cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER "/usr/bin/clang-10" )
set(CMAKE_CXX_COMPILER "/usr/bin/clang++-10")
add_definitions(-D__STRICT_ANSI__)

project(CoroutineExample)

set(CMAKE_CXX_FLAGS "-fcoroutines-ts -stdlib=libc++")

include_directories(../../src)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)
find_package(Boost 1.65.0 REQUIRED system)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif(Boost_FOUND)

link_directories(/usr/local/lib)
add_subdirectory(../../ ${CMAKE_BINARY_DIR}/bin)
add_executable(CoroutineExample main.cpp)
target_link_libraries(CoroutineExample ICC pthread ${Boost_SYSTEM_LIBRARY})