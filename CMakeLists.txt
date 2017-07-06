cmake_minimum_required(VERSION 3.7)
project(ICC)

set(CMAKE_CXX_STANDARD 14)

include_directories(./src)

set(CMAKE_C_COMPILER "clang" )
set(CMAKE_CXX_COMPILER "clang++" )
add_definitions(-D__STRICT_ANSI__)
set(SOURCE_FILES    main.cpp
                    src/IComponent.hpp
                    src/Event.hpp
                    src/Timer.hpp
                    src/ITimerLisener.hpp
                    src/service/IService.hpp
                    src/service/IClient.hpp
                    src/service/ProcessBus.cpp
                    src/service/ProcessBus.hpp
                    src/helpers/memory_helper.hpp
                    InterfaceForinterface.hpp)
add_executable(ICC ${SOURCE_FILES})
target_link_libraries(ICC boost_system
                          pthread
                     )