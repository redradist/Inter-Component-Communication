cmake_minimum_required(VERSION 3.7)
project(CommonAPI_Server)

set(CMAKE_CXX_STANDARD  11 )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")

include_directories(../../src)
include_directories(/usr/local/include/CommonAPI-3.1)
include_directories(/usr/local/lib/dbus-1.0/include)
include_directories(/usr/include/dbus-1.0)
include_directories(${CMAKE_SOURCE_DIR}/cmake-build-debug/src-gen)

link_directories(/usr/local/lib)

file(GLOB FIDL_FILES ./interfaces/*.fidl)
message(STATUS "FIDL_FILES is ${FIDL_FILES}")
message(STATUS "BUBUB is ${BUBUB}")
foreach(FIDL ${FIDL_FILES})
    message(STATUS "FIDL is ${FIDL}")
    get_filename_component(FIDL_NAME_WITH_EXTENTION ${FIDL} NAME)
    string(REPLACE ".fidl" "" FIDL_NAME ${FIDL_NAME_WITH_EXTENTION})
    message(STATUS "FIDL_NAME is ${FIDL_NAME}")
    add_custom_command(
            OUTPUT ${FIDL_NAME}
            DEPENDS ${FIDL}
            COMMAND touch ${FIDL_NAME}
            COMMAND $ENV{HOME}/cgen/commonapi-generator/commonapi-generator-linux-x86_64 -sk ${FIDL}
            COMMAND $ENV{HOME}/cgen/commonapi_dbus_generator/commonapi-dbus-generator-linux-x86_64 ${FIDL}
    )
    set(GEN_FIDL_FILES ${GEN_FIDL_FILES} ${FIDL_NAME})
endforeach()
message(STATUS "GEN_FIDL_FILES is ${GEN_FIDL_FILES}")

add_custom_target(fidl_gen
        DEPENDS ${GEN_FIDL_FILES}
        COMMAND echo "Do it ...")

set(SOURCE_FILES main.cpp HelloWorldStubImpl.cpp HelloWorldStubImpl.hpp)
set(GENERATED_FILES
        "${CMAKE_BINARY_DIR}/src-gen/v1/commonapi/HelloWorldDBusProxy.cpp"
        "${CMAKE_BINARY_DIR}/src-gen/v1/commonapi/HelloWorldDBusDeployment.cpp"
        "${CMAKE_BINARY_DIR}/src-gen/v1/commonapi/HelloWorldDBusStubAdapter.cpp"
        "${CMAKE_BINARY_DIR}/src-gen/v1/commonapi/HelloWorldStubDefault.cpp"
        "${CMAKE_BINARY_DIR}/src-gen/v1/commonapi/HelloWorldDBusDeployment.cpp")
set_source_files_properties(${GENERATED_FILES} PROPERTIES GENERATED TRUE)

add_subdirectory(../../ ${CMAKE_BINARY_DIR}/bin)
add_executable(
        CommonAPI_Server
        ${SOURCE_FILES}
        ${GENERATED_FILES})
add_dependencies(CommonAPI_Server ICC)
add_dependencies(CommonAPI_Server fidl_gen)
target_link_libraries(CommonAPI_Server CommonAPI CommonAPI-DBus dbus-1)