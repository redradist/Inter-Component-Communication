cmake_minimum_required(VERSION 3.7)
project(CommandLoop)

set(CMAKE_CXX_STANDARD 11)

include_directories(../../src)

set(SOURCE_FILES    main.cpp)
add_subdirectory(../../ ${CMAKE_BINARY_DIR}/bin)
add_executable(CommandLoop ${SOURCE_FILES})
add_dependencies(CommandLoop ICC)
target_link_libraries(CommandLoop boost_system
        pthread
        ICC
        )