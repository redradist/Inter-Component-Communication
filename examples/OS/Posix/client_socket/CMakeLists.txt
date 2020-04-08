cmake_minimum_required(VERSION 3.13)
project(PosixClientSocket)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(../../../.. ${CMAKE_BINARY_DIR}/bin)

add_executable(PosixClientSocket main.cpp)
target_link_libraries(PosixClientSocket ICC)