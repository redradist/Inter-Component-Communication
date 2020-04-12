cmake_minimum_required(VERSION ${CMAKE_VERSION})
#NOTE(redra): This commented lines only for testing purposes
#set(CMAKE_C_COMPILER "clang")
#set(CMAKE_CXX_COMPILER "clang++")
#add_definitions(-D__STRICT_ANSI__)
#set(CMAKE_C_COMPILER "gcc-4.8")
#set(CMAKE_CXX_COMPILER "g++-4.8")
#set(CMAKE_C_COMPILER "gcc-4.9")
#set(CMAKE_CXX_COMPILER "g++-4.9")
project(ICC)

message(STATUS "CMAKE_CXX_STANDARD is ${CMAKE_CXX_STANDARD}")
set(ICC_SUPPORTED_CXX_STANDARDS "11;14;17;20")
if (NOT CMAKE_CXX_STANDARD)
    message(FATAL_ERROR "Cannot detect C++ Standard. Please, set variable CMAKE_CXX_STANDARD !!")
endif()
if (NOT CMAKE_CXX_STANDARD IN_LIST ICC_SUPPORTED_CXX_STANDARDS)
    message(FATAL_ERROR "ICC library does not support ${CMAKE_CXX_STANDARD}")
endif()
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-type")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-stack-address")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-local-addr")
    endif()
endif()
set(ICC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE ICC_SOURCE_FILES src/*.c src/*.cpp)
add_library(${PROJECT_NAME} ${ICC_SOURCE_FILES})
target_include_directories(${PROJECT_NAME}
                           PUBLIC ${ICC_INCLUDE_DIR}
                           PRIVATE ${ICC_INCLUDE_DIR}/icc/_private)
target_link_libraries(${PROJECT_NAME} pthread)

add_subdirectory(test)
