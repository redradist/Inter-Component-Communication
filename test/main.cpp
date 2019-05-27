/**
 * @file main.cpp
 * @author Denis Kotov
 * @date 27 May 2019
 * @brief Default launcher of Google Test Framework
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#include <gtest/gtest.h>

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
