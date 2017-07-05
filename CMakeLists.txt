cmake_minimum_required(VERSION 3.7)
project(ICC)

set(CMAKE_CXX_STANDARD 11)

include_directories(./src)

set(SOURCE_FILES    src/service/ProcessBus.cpp)
add_library(ICC ${SOURCE_FILES})
target_link_libraries(ICC boost_system
                          pthread
                     )