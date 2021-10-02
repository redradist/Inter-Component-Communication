cmake_minimum_required(VERSION 3.7)
project(Forecast)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_subdirectory(../../ ${CMAKE_BINARY_DIR}/bin)
add_executable(Forecast ${SOURCE_FILES})
add_dependencies(Forecast ICC_static)
target_link_libraries(Forecast pthread ICC)