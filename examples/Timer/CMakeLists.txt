cmake_minimum_required(VERSION 3.7)
project(Timer)

set(CMAKE_CXX_STANDARD 11)

include_directories(../../src)

set(SOURCE_FILES    main.cpp)
add_subdirectory(../../ ${CMAKE_BINARY_DIR}/bin)
add_executable(Timer ${SOURCE_FILES})
add_dependencies(Timer ICC)
target_link_libraries(Timer boost_system
                            pthread
                            ICC
                            )