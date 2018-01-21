cmake_minimum_required(VERSION 3.7)
# This comented lines only for testing purposes
#set(CMAKE_C_COMPILER "clang" )
#set(CMAKE_CXX_COMPILER "clang++" )
#add_definitions(-D__STRICT_ANSI__)
#set(CMAKE_C_COMPILER "gcc-4.8" )
#set(CMAKE_CXX_COMPILER "g++-4.8" )
#set(CMAKE_C_COMPILER "gcc-4.9" )
#set(CMAKE_CXX_COMPILER "g++-4.9" )
project(ICC)

set(CMAKE_CXX_STANDARD 11)

set(ICC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src PARENT_SCOPE)
set(ICC_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

include_directories(${ICC_INCLUDE_DIR})
set(SOURCE_FILES    src/service/ProcessBus.cpp
                    src/command/CommandLoop.cpp src/command/Builder.hpp src/command/ICommandLoop.hpp)

add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME}
                           PUBLIC ${ICC_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} boost_system
                                      pthread)