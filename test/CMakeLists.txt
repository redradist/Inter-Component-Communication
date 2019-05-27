project(icc_tests)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Werror=return-type -Werror=return-local-addr")

################################
# GTest
################################
add_subdirectory (../libs/googletest icc/tests/googletest)
enable_testing()
include_directories(${ICC_INCLUDE_DIR})
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

file(GLOB_RECURSE ICC_TEST_SOURCE_FILES icc/*.cpp)

################################
# Unit Tests
################################
# Add test cpp file
add_executable(${PROJECT_NAME} ${ICC_TEST_SOURCE_FILES})

# Link test executable against gtest & gtest_main
target_link_libraries(${PROJECT_NAME} gtest gtest_main)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${PROJECT_NAME})
