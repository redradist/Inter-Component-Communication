cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER "/usr/bin/clang-9" )
set(CMAKE_CXX_COMPILER "/usr/bin/clang++-9")
add_definitions(-D__STRICT_ANSI__)

project(CommonAPI_Server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(COMMONAPI_GENERATOR $ENV{HOME}/Projects/Automotive/cgen/commonapi-generator/commonapi-generator-linux-x86_64)
set(COMMONAPI_DBUS_GENERATOR $ENV{HOME}/Projects/Automotive/cgen/commonapi_dbus_generator/commonapi-dbus-generator-linux-x86_64)

include(../../src/icc/interprocess/commonapi/cmake-config/wrapper_generator.cmake)
include_directories(../../src)

include_directories(${CMAKE_BINARY_DIR})
include_directories($ENV{HOME}/Projects/Automotive/capicxx-dbus-runtime/include/CommonAPI/DBus)
include_directories(/usr/local/boost_1_69_0/include)
include_directories(../../src)
include_directories(/usr/local/include/CommonAPI-3.1)
include_directories(/usr/local/lib/dbus-1.0/include)
include_directories(/usr/include/dbus-1.0)
include_directories(/home/redra/Projects/Automotive/dbus-1.11.12/build)
include_directories(${CMAKE_SOURCE_DIR}/cmake-build-debug/src-gen)
include_directories(
        $ENV{HOME}/Projects/Automotive/capicxx-core-runtime/include
        $ENV{HOME}/Projects/Automotive/capicxx-dbus-runtime/include
        $ENV{HOME}/Projects/Automotive/dbus-1.11.12
        /usr/lib/x86_64-linux-gnu/dbus-1.0/include/
)

link_directories(
        $ENV{HOME}/Projects/Automotive/capicxx-core-runtime/build
        $ENV{HOME}/Projects/Automotive/capicxx-dbus-runtime/build
        $ENV{HOME}/Projects/Automotive/dbus-1.11.12/build/dbus/.libs
)
link_directories(/usr/local/boost_1_69_0/lib)
link_directories(/usr/local/lib)

file(GLOB FIDL_FILES ./interfaces/*.fidl)
set(GEN_DIR "${CMAKE_BINARY_DIR}")
set(SOURCE_FILES main.cpp HelloWorldStubImpl.cpp HelloWorldStubImpl.hpp)
#set(GENERATED_FILES
#        "${CMAKE_BINARY_DIR}/src-gen/v1/commonapi/HelloWorldDBusProxy.cpp"
#        "${CMAKE_BINARY_DIR}/src-gen/v1/commonapi/HelloWorldDBusDeployment.cpp"
#        "${CMAKE_BINARY_DIR}/src-gen/v1/commonapi/HelloWorldDBusStubAdapter.cpp"
#        "${CMAKE_BINARY_DIR}/src-gen/v1/commonapi/HelloWorldStubDefault.cpp"
#        "${CMAKE_BINARY_DIR}/src-gen/v1/commonapi/HelloWorldDBusDeployment.cpp")
#set_source_files_properties(${GENERATED_FILES} PROPERTIES GENERATED TRUE)

add_subdirectory(../../ ${CMAKE_BINARY_DIR}/bin)
message(STATUS "ICC_INCLUDE_DIR is ${ICC_INCLUDE_DIR}")

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${GENERATED_FILES})
add_dependencies(${PROJECT_NAME} ICC)
add_wrapper_dependencies(${PROJECT_NAME} "${FIDL_FILES}" Service ${CMAKE_BINARY_DIR})
add_wrapper_dependencies(${PROJECT_NAME} "${FIDL_FILES}" Client ${CMAKE_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} ICC CommonAPI CommonAPI-DBus dbus-1 boost_system)
