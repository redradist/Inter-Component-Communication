cmake_minimum_required(VERSION 3.16)
project(Optional)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-D__cpp_lib_optional=201606)
add_subdirectory(../../.. ${CMAKE_BINARY_DIR}/bin)

add_executable(Optional main.cpp)
target_link_libraries(Optional ICC)