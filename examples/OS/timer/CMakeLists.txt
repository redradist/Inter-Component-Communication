cmake_minimum_required(VERSION 3.13)
project(OSTimer)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(../../../ ${CMAKE_BINARY_DIR}/bin)

add_executable(OSTimer main.cpp)
target_link_libraries(OSTimer ICC_static ws2_32 wsock32)
#set_property(TARGET OSTimer PROPERTY
#             MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")