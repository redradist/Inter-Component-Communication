cmake_minimum_required(VERSION 3.7)
project(ICC)

set(CMAKE_CXX_STANDARD 11)

include_directories(./src)

# This comented lines only for testing purposes
#set(CMAKE_C_COMPILER "clang" )
#set(CMAKE_CXX_COMPILER "clang++" )
#add_definitions(-D__STRICT_ANSI__)
#set(CMAKE_C_COMPILER "gcc-4.8" )
#set(CMAKE_CXX_COMPILER "g++-4.8" )
#set(CMAKE_C_COMPILER "gcc-4.9" )
#set(CMAKE_CXX_COMPILER "g++-4.9" )
set(SOURCE_FILES    src/service/ProcessBus.cpp
                    src/command/CommandLoop.cpp)
add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(
        ${PROJECT_NAME}
        boost_system
        pthread
)