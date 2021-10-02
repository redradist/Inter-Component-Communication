cmake_minimum_required(VERSION 3.13)
project(ServerSocket)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(../../.. ${CMAKE_BINARY_DIR}/bin)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ICC_static)