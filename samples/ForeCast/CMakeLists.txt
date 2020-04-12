cmake_minimum_required(VERSION 3.7)
project(Forecast)

set(CMAKE_CXX_STANDARD 11)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)
find_package(Boost 1.65.0 REQUIRED system)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif(Boost_FOUND)

set(SOURCE_FILES main.cpp)
add_subdirectory(../../ ${CMAKE_BINARY_DIR}/bin)
add_executable(Forecast ${SOURCE_FILES})
add_dependencies(Forecast ICC)
target_link_libraries(Forecast pthread ICC ${Boost_SYSTEM_LIBRARY})