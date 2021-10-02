cmake_minimum_required(VERSION 3.7)
project(TreadPool)

set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_BINARY_DIR})
include_directories($ENV{HOME}/Projects/Automotive/capicxx-dbus-runtime/include/CommonAPI/DBus)
include_directories(/usr/local/lib/boost/include)
include_directories(/usr/local/include/CommonAPI-3.1)
include_directories(/usr/local/lib/dbus-1.0/include)
include_directories(/usr/include/dbus-1.0)
include_directories(${CMAKE_SOURCE_DIR}/cmake-build-debug/src-gen)
include_directories(
        $ENV{HOME}/Projects/Automotive/capicxx-core-runtime/include
        $ENV{HOME}/Projects/Automotive/capicxx-dbus-runtime/include
        $ENV{HOME}/Projects/Automotive/dbus-1.11.12
        /usr/lib/x86_64-linux-gnu/dbus-1.0/include/
)

link_directories(/usr/local/lib/boost/lib)
link_directories(/usr/local/lib)

set(SOURCE_FILES main.cpp)
add_subdirectory(../../ ${CMAKE_BINARY_DIR}/bin)
add_executable(TreadPool ${SOURCE_FILES})
add_dependencies(TreadPool ICC_static)
if (UNIX)
    target_link_libraries(TreadPool ICC_static pthread)
elseif(WIN32)
    target_link_libraries(TreadPool ICC_static ws2_32 wsock32)
endif()