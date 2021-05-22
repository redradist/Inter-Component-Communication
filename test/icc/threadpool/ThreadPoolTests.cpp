/**
 * @file ThreadPoolTests.cpp
 * @author Denis Kotov
 * @date 22 May 2021
 * @brief Contains tests for ThreadPool class
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#include <gtest/gtest.h>
#include <unordered_set>

#include <icc/threadpool/ThreadPool.hpp>

using namespace std::chrono_literals;

struct ThreadPoolTest : testing::Test
{
  void SetUp() override {
  };

  void TearDown() override {
  }

  const size_t hardware_concurrency_ = std::thread::hardware_concurrency();
};

TEST_F(ThreadPoolTest, HundredMoreJobs_DefaultThreadPool_Success)
{
  auto& threadPool = icc::threadpool::ThreadPool::getDefaultPool();
  std::unordered_set<std::thread::id> unique_threads;
  std::mutex mtx;
  std::condition_variable cond_var;
  const unsigned numJobs = 100 * hardware_concurrency_;
  unsigned numHandledJobs = 0;
  for (int i = 0; i < numJobs; ++i) {
    threadPool.push([&mtx, &cond_var, &numHandledJobs, &unique_threads] {
      std::this_thread::sleep_for(100ms);
      {
        std::lock_guard<std::mutex> lock{mtx};
        numHandledJobs += 1;
        unique_threads.insert(std::this_thread::get_id());
      }
      cond_var.notify_one();
    });
  }
  std::unique_lock<std::mutex> lock{mtx};
  cond_var.wait(lock, [&numHandledJobs, &numJobs] {
    return numHandledJobs == numJobs;
  });
  ASSERT_EQ(unique_threads.size(), hardware_concurrency_);
  ASSERT_EQ(numHandledJobs, numJobs);
}

TEST_F(ThreadPoolTest, HundredMoreJobs_ThreadPool_Success)
{
  auto threadPool = icc::threadpool::ThreadPool::createPool();
  std::unordered_set<std::thread::id> unique_threads;
  std::mutex mtx;
  std::condition_variable cond_var;
  const unsigned numJobs = 100 * hardware_concurrency_;
  unsigned numHandledJobs = 0;
  for (int i = 0; i < numJobs; ++i) {
    threadPool->push([&mtx, &cond_var, &numHandledJobs, &unique_threads] {
      std::this_thread::sleep_for(10ms);
      {
        std::lock_guard<std::mutex> lock{mtx};
        numHandledJobs += 1;
        unique_threads.insert(std::this_thread::get_id());
      }
      cond_var.notify_one();
    });
  }
  std::unique_lock<std::mutex> lock{mtx};
  cond_var.wait_for(lock, 110s, [&numHandledJobs, &numJobs] {
    return numHandledJobs == numJobs;
  });
  ASSERT_EQ(unique_threads.size(), hardware_concurrency_);
  ASSERT_EQ(numHandledJobs, numJobs);
}