cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(CoroutineExample)

add_subdirectory(../../ ${CMAKE_BINARY_DIR}/bin)
add_executable(CoroutineExample main.cpp)
target_link_libraries(CoroutineExample ICC_static)
