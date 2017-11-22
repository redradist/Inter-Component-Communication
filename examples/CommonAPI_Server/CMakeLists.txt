cmake_minimum_required(VERSION 3.7)
project(CommonAPI_Server)

set(CMAKE_CXX_STANDARD  11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")

include(../../src/commonapi/fidl_generator.cmake)
include_directories(../../src)

include_directories(${CMAKE_BINARY_DIR})
include_directories(/usr/local/include/CommonAPI-3.1)
include_directories(/usr/local/lib/dbus-1.0/include)
include_directories(/usr/include/dbus-1.0)
include_directories(${CMAKE_SOURCE_DIR}/cmake-build-debug/src-gen)

link_directories(/usr/local/lib)

file(GLOB FIDL_FILES ./interfaces/*.fidl)
set(SOURCE_FILES main.cpp HelloWorldStubImpl.cpp HelloWorldStubImpl.hpp)
set(GENERATED_FILES
        "${CMAKE_BINARY_DIR}/src-gen/v1/commonapi/HelloWorldDBusProxy.cpp"
        "${CMAKE_BINARY_DIR}/src-gen/v1/commonapi/HelloWorldDBusDeployment.cpp"
        "${CMAKE_BINARY_DIR}/src-gen/v1/commonapi/HelloWorldDBusStubAdapter.cpp"
        "${CMAKE_BINARY_DIR}/src-gen/v1/commonapi/HelloWorldStubDefault.cpp"
        "${CMAKE_BINARY_DIR}/src-gen/v1/commonapi/HelloWorldDBusDeployment.cpp")
set_source_files_properties(${GENERATED_FILES} PROPERTIES GENERATED TRUE)

add_subdirectory(../../ ${CMAKE_BINARY_DIR}/bin)
message(STATUS "ICC_INCLUDE_DIR is ${ICC_INCLUDE_DIR}")

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${GENERATED_FILES})
add_dependencies(${PROJECT_NAME} ICC)
add_fidl_dependencies(${PROJECT_NAME} ${FIDL_FILES} $ENV{HOME}/cgen)
target_link_libraries(${PROJECT_NAME} CommonAPI CommonAPI-DBus dbus-1)
