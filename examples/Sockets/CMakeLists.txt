cmake_minimum_required(VERSION 3.15)
project(Posix)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(../../ ${CMAKE_BINARY_DIR}/bin)

add_executable(Sockets main.cpp)
target_link_libraries(Sockets ICC)