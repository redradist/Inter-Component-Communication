cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(ICC)

option(ICC_BUILD_SHARED   "Build ICC as a shared library" OFF)
option(ICC_BUILD_TESTS    "Build tests" OFF)

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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-type -Werror=old-style-cast")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-stack-address")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-local-addr")
    endif()
endif()
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHs")
endif()
set(ICC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE ICC_SOURCE_FILES src/*.c src/*.cpp)
message(STATUS "ICC_SOURCE_FILES is ${ICC_SOURCE_FILES}")
set(ICC_OS_HEADERS "")
if (UNIX)
    list(FILTER ICC_SOURCE_FILES EXCLUDE REGEX "src/icc/os/platforms/windows/.*")
    set(ICC_OS_HEADERS "${ICC_INCLUDE_DIR}/icc/os/platforms/posix")
elseif(WIN32)
    list(FILTER ICC_SOURCE_FILES EXCLUDE REGEX "src/icc/os/platforms/posix/.*")
    set(ICC_OS_HEADERS "${ICC_INCLUDE_DIR}/icc/os/platforms/windows")
endif ()
message(STATUS "ICC_SOURCE_FILES is ${ICC_SOURCE_FILES}")

if(NOT EXISTS libs/commonapi_tools)
    execute_process(
            COMMAND git submodule update --init --recursive libs/commonapi_tools
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
endif()

if(ICC_BUILD_SHARED)
    add_library(${PROJECT_NAME} SHARED ${ICC_SOURCE_FILES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/icc/_private/dynamic_lib)
    target_compile_definitions(${PROJECT_NAME} PUBLIC ICC_SHARED_LIBRARY)
    set(ICC_LIBRARY_NAME ${PROJECT_NAME})
else()
    add_library(${PROJECT_NAME}_static STATIC ${ICC_SOURCE_FILES})
    target_include_directories(${PROJECT_NAME}_static PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/icc/_private/static_lib)
    set(ICC_LIBRARY_NAME ${PROJECT_NAME}_static)
endif()
target_compile_definitions(${ICC_LIBRARY_NAME} PRIVATE -DICC_LIBRARY)
target_include_directories(${ICC_LIBRARY_NAME}
                           PUBLIC ${ICC_INCLUDE_DIR}
                           PRIVATE ${ICC_INCLUDE_DIR}/icc/_private
                                   ${ICC_OS_HEADERS})
if (UNIX)
    target_link_libraries(${ICC_LIBRARY_NAME} pthread)
    install(TARGETS ${ICC_LIBRARY_NAME} DESTINATION lib)
    install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/src/icc
            DESTINATION include
            FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
                           PATTERN "icc/os/platforms/windows/*" EXCLUDE
                           PATTERN "icc/_private/*_lib" EXCLUDE)
elseif(WIN32)
    target_link_libraries(${ICC_LIBRARY_NAME} ws2_32 wsock32)
    if(ICC_BUILD_SHARED)
        install(TARGETS ${ICC_LIBRARY_NAME} RUNTIME DESTINATION bin)
        install(TARGETS ${ICC_LIBRARY_NAME} LIBRARY DESTINATION lib)
    else()
        install(TARGETS ${ICC_LIBRARY_NAME} DESTINATION lib)
    endif()
    install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/src/icc
            DESTINATION include
            FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
                           PATTERN "icc/os/platforms/posix/*" EXCLUDE
                           PATTERN "icc/_private/*_lib" EXCLUDE)
endif()
if(ICC_BUILD_SHARED)
    install(FILES ${CMAKE_CURRENT_LIST_DIR}/src/icc/_private/dynamic_lib/api_trigger.hpp DESTINATION include/icc/_private/)
else()
    install(FILES ${CMAKE_CURRENT_LIST_DIR}/src/icc/_private/static_lib/api_trigger.hpp DESTINATION include/icc/_private/)
endif()
install(FILES LICENSE DESTINATION licenses)

if(ICC_BUILD_TESTS)
    if(NOT EXISTS libs/googletest)
        execute_process(
                COMMAND git submodule update --init --recursive libs/googletest
                WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
    endif()

    add_subdirectory(test)
endif()