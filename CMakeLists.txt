cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(ICC)

if (NOT CMAKE_CXX_STANDARD)
    message(STATUS "Cannot detect C++ Standard. Switching to C++11 by default !!")
    set(CMAKE_CXX_STANDARD 11)
endif()
message(STATUS "CMAKE_CXX_STANDARD is ${CMAKE_CXX_STANDARD}")
set(ICC_SUPPORTED_CXX_STANDARDS "11;14;17;20")
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
message(STATUS "ICC_SOURCE_FILES is ${ICC_SOURCE_FILES}")
if (UNIX)
    list(FILTER ICC_SOURCE_FILES EXCLUDE REGEX "src/icc/os/platforms/windows/.*")
elseif(WIN32)
    list(FILTER ICC_SOURCE_FILES EXCLUDE REGEX "src/icc/os/platforms/posix/.*")
endif ()
message(STATUS "ICC_SOURCE_FILES is ${ICC_SOURCE_FILES}")

add_library(${PROJECT_NAME} ${ICC_SOURCE_FILES} src/icc/os/platforms/windows/Common.hpp)
target_include_directories(${PROJECT_NAME}
                           PUBLIC ${ICC_INCLUDE_DIR}
                           PRIVATE ${ICC_INCLUDE_DIR}/icc/_private)
target_link_libraries(${PROJECT_NAME} pthread)

add_subdirectory(test)
