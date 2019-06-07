/**
 * @file TimerTests.hpp
 * @author Denis Kotov
 * @date 13 May 2019
 * @brief Contains tests for Timer class
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#include <gtest/gtest.h>
#include <atomic>
#include <thread>
#include <icc/os/posix/TimerImpl.hpp>

struct TimerTest : testing::Test
{
  virtual void SetUp() {

  };

  virtual void TearDown() {

  }
};

TEST_F(TimerTest, TenMillionItems_TenReadThread_OneWriteThread)
{
  auto timer = icc::os::posix::Timer::createTimer();

  timer->start(10000);
  std::this_thread::sleep_for(std::chrono::seconds(20));
}
