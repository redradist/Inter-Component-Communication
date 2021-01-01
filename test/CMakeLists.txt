project(icc_tests)

################################
# GTest
################################
add_subdirectory (../libs/googletest icc/tests/googletest)
enable_testing()
include_directories(${ICC_INCLUDE_DIR})
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

file(GLOB_RECURSE ICC_TEST_SOURCE_FILES icc/*.cpp)
set(ICC_OS_HEADERS "")
if (UNIX)
    list(FILTER ICC_TEST_SOURCE_FILES EXCLUDE REGEX "icc/os/platforms/windows/.*")
elseif(WIN32)
    list(FILTER ICC_TEST_SOURCE_FILES EXCLUDE REGEX "icc/os/platforms/posix/.*")
endif ()

################################
# Unit Tests
################################
# Add test cpp file
add_executable(${PROJECT_NAME} ${ICC_TEST_SOURCE_FILES})

# Link test executable against gtest & gtest_main
target_link_libraries(${PROJECT_NAME} gtest gtest_main ICC)
