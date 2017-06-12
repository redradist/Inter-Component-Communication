cmake_minimum_required(VERSION 3.7)
project(ICC)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES    main.cpp
                    src/Component.hpp
                    src/Event.hpp
                    src/Timer.hpp
                    src/TimerLisener.hpp
                    src/EventLoopProvider.hpp)
add_executable(ICC ${SOURCE_FILES})
target_link_libraries(ICC boost_system
                          pthread
                     )