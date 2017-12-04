cmake_minimum_required(VERSION 3.7)
# This comented lines only for testing purposes
#set(CMAKE_C_COMPILER "clang" )
#set(CMAKE_CXX_COMPILER "clang++" )
#add_definitions(-D__STRICT_ANSI__)
#set(CMAKE_C_COMPILER "gcc-4.8" )
#set(CMAKE_CXX_COMPILER "g++-4.8" )
#set(CMAKE_C_COMPILER "gcc-4.9" )
#set(CMAKE_CXX_COMPILER "g++-4.9" )
project(CommonAPI_Client)

set(CMAKE_CXX_STANDARD  11 )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")

set(COMMONAPI_GENERATOR $ENV{HOME}/cgen/commonapi-generator/commonapi-generator-linux-x86_64)
set(COMMONAPI_DBUS_GENERATOR $ENV{HOME}/cgen/commonapi_dbus_generator/commonapi-dbus-generator-linux-x86_64)

include(../../src/commonapi/fidl_generator.cmake)

include_directories(${CMAKE_BINARY_DIR})
include_directories(/home/redra/Projects/Automotive/capicxx-dbus-runtime/include/CommonAPI/DBus)
include_directories(/usr/local/lib/boost/include)
include_directories(../../src)
include_directories(/usr/local/include/CommonAPI-3.1)
include_directories(/usr/local/lib/dbus-1.0/include)
include_directories(/usr/include/dbus-1.0)
include_directories(${CMAKE_SOURCE_DIR}/cmake-build-debug/src-gen)
include_directories(
        /home/redra/Projects/Automotive/capicxx-core-runtime/include
        /home/redra/Projects/Automotive/capicxx-dbus-runtime/include
        /home/redra/Projects/Automotive/dbus-1.11.12
        /usr/lib/x86_64-linux-gnu/dbus-1.0/include/
)

link_directories(
        /home/redra/Projects/Automotive/capicxx-core-runtime/build
        /home/redra/Projects/Automotive/capicxx-dbus-runtime/build
        /home/redra/Projects/Automotive/dbus-1.11.12/build/dbus/.libs
)
link_directories(/usr/local/lib/boost/lib)
link_directories(/usr/local/lib)

file(GLOB FIDL_FILES ./interfaces/HelloWorld.fidl)

#file(GLOB FIDL_FILES ./interfaces/I*.fidl)
#list(FILTER FIDL_FILES INCLUDE REGEX ./interfaces/I*.fidl)
message(STATUS "FIDL_FILES is ${FIDL_FILES}")

set(SOURCE_FILES main.cpp HelloWorldProxyClient.cpp HelloWorldProxyClient.hpp)
#set(GENERATED_FILES
#        "${CMAKE_BINARY_DIR}/v1/commonapi/HelloWorldDBusProxy.cpp"
#        "${CMAKE_BINARY_DIR}/v1/commonapi/HelloWorldDBusProxy.cpp"
#        "${CMAKE_BINARY_DIR}/v1/commonapi/HelloWorldDBusDeployment.cpp"
#        "${CMAKE_BINARY_DIR}/v1/commonapi/HelloWorldDBusStubAdapter.cpp"
#        "${CMAKE_BINARY_DIR}/v1/commonapi/HelloWorldStubDefault.cpp"
#        "${CMAKE_BINARY_DIR}/v1/commonapi/HelloWorldDBusDeployment.cpp")
#set_source_files_properties(${GENERATED_FILES} PROPERTIES GENERATED TRUE)

#add_custom_command(
#        OUTPUT ${GENERATED_FILES}
#        DEPENDS ${FIDL_FILES}
#        COMMAND ${COMMONAPI_GENERATOR} --dest ${GENERATOR_PATH} -sk ${FIDL_FILES}
#        COMMAND ${COMMONAPI_DBUS_GENERATOR} --dest ${GENERATOR_PATH} ${FIDL_FILES}
#)
add_custom_target(
        afdsaasdadsdas_commonapi_gen
        DEPENDS ${GENERATED_FILES}
        COMMAND echo "Generation of CommonAPI wrappers is finished")

add_subdirectory(../../ ${CMAKE_BINARY_DIR}/bin)
add_executable(
        CommonAPI_Client
        ${SOURCE_FILES}
        ${GENERATED_FILES})
add_dependencies(CommonAPI_Client ICC afdsaasdadsdas_commonapi_gen)
add_fidl_dependencies(CommonAPI_Client ${FIDL_FILES} Client ${CMAKE_BINARY_DIR})
target_link_libraries(CommonAPI_Client CommonAPI CommonAPI-DBus dbus-1 boost_system)