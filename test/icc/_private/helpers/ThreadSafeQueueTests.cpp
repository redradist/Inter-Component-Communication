/**
 * @file ThreadSafeQueueTests.hpp
 * @author Denis Kotov
 * @date 13 May 2019
 * @brief Contains tests for ThreadSafeQueue class
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#include <gtest/gtest.h>
#include <atomic>
#include <thread>
#include <icc/_private/containers/ThreadSafeQueue.hpp>

template <typename TItem>
using ThreadSafeQueue = icc::_private::containers::ThreadSafeQueue<TItem>;

struct ThreadSafeQueueIntTest : testing::Test
{
  std::atomic_uint write_count{0};
  std::atomic_uint read_count{0};
  std::shared_ptr<ThreadSafeQueue<int>> queue;
  long long max_items = 0;
  long long num_read_threads = 0;
  long long num_write_threads = 0;

  virtual void SetUp() {
    write_count.store(0);
    read_count.store(0);
    queue = std::make_shared<ThreadSafeQueue<int>>();
  };

  virtual void TearDown() {
    queue.reset();
    write_count.store(0);
    read_count.store(0);
  }

  void writeQueue() {
    for (int i = 0; i < max_items; ++i) {
      queue->push(i);
      write_count++;
    }
  }

  void readQueue() {
    while (read_count.load() < num_write_threads * max_items) {
      int it;
      if (queue->tryPop(it)) {
        read_count.fetch_add(1);
      }
    }
  }
};

TEST_F(ThreadSafeQueueIntTest, TenMillionItems_TenReadThreads_OneWriteThread)
{
  max_items = 10000000;
  num_read_threads = 10;
  num_write_threads = 1;

  clock_t tStart = clock();

  std::vector<std::thread> readThreads;
  for (int i = 0; i < num_read_threads; ++i) {
    readThreads.emplace_back(&ThreadSafeQueueIntTest::readQueue, this);
  }

  std::vector<std::thread> writeThreads;
  for (int i = 0; i < num_write_threads; ++i) {
    writeThreads.emplace_back(&ThreadSafeQueueIntTest::writeQueue, this);
  }

  for (int i = 0; i < num_read_threads; ++i) {
    readThreads[i].join();
  }

  for (int i = 0; i < num_write_threads; ++i) {
    writeThreads[i].join();
  }

  printf("Time taken: %.2fs\n", static_cast<double>((clock() - tStart)/CLOCKS_PER_SEC));

  std::cout << "Write data = " << write_count.load() << std::endl;
  std::cout << "Read data = " << read_count.load() << std::endl;

  EXPECT_EQ(write_count, read_count);
}

TEST_F(ThreadSafeQueueIntTest, TenMillionItems_OneReadThread_TenWriteThreads)
{
  max_items = 10000000;
  num_read_threads = 1;
  num_write_threads = 10;

  clock_t tStart = clock();

  std::vector<std::thread> readThreads;
  for (int i = 0; i < num_read_threads; ++i) {
    readThreads.emplace_back(&ThreadSafeQueueIntTest::readQueue, this);
  }

  std::vector<std::thread> writeThreads;
  for (int i = 0; i < num_write_threads; ++i) {
    writeThreads.emplace_back(&ThreadSafeQueueIntTest::writeQueue, this);
  }

  for (int i = 0; i < num_read_threads; ++i) {
    readThreads[i].join();
  }

  for (int i = 0; i < num_write_threads; ++i) {
    writeThreads[i].join();
  }

  printf("Time taken: %.2fs\n", static_cast<double>((clock() - tStart)/CLOCKS_PER_SEC));

  std::cout << "Write data = " << write_count.load() << std::endl;
  std::cout << "Read data = " << read_count.load() << std::endl;

  EXPECT_EQ(write_count, read_count);
}

TEST_F(ThreadSafeQueueIntTest, TenMillionItems_FiveReadThreads_TenWriteThreads)
{
  max_items = 10000000;
  num_read_threads = 5;
  num_write_threads = 10;

  clock_t tStart = clock();

  std::vector<std::thread> readThreads;
  for (int i = 0; i < num_read_threads; ++i) {
    readThreads.emplace_back(&ThreadSafeQueueIntTest::readQueue, this);
  }

  std::vector<std::thread> writeThreads;
  for (int i = 0; i < num_write_threads; ++i) {
    writeThreads.emplace_back(&ThreadSafeQueueIntTest::writeQueue, this);
  }

  for (int i = 0; i < num_read_threads; ++i) {
    readThreads[i].join();
  }

  for (int i = 0; i < num_write_threads; ++i) {
    writeThreads[i].join();
  }

  printf("Time taken: %.2fs\n", static_cast<double>((clock() - tStart)/CLOCKS_PER_SEC));

  std::cout << "Write data = " << write_count.load() << std::endl;
  std::cout << "Read data = " << read_count.load() << std::endl;

  EXPECT_EQ(write_count, read_count);
}

TEST_F(ThreadSafeQueueIntTest, TenMillionItems_TenReadThreads_FiveWriteThreads)
{
  max_items = 10000000;
  num_read_threads = 10;
  num_write_threads = 5;

  clock_t tStart = clock();

  std::vector<std::thread> readThreads;
  for (int i = 0; i < num_read_threads; ++i) {
    readThreads.emplace_back(&ThreadSafeQueueIntTest::readQueue, this);
  }

  std::vector<std::thread> writeThreads;
  for (int i = 0; i < num_write_threads; ++i) {
    writeThreads.emplace_back(&ThreadSafeQueueIntTest::writeQueue, this);
  }

  for (int i = 0; i < num_read_threads; ++i) {
    readThreads[i].join();
  }

  for (int i = 0; i < num_write_threads; ++i) {
    writeThreads[i].join();
  }

  printf("Time taken: %.2fs\n", static_cast<double>((clock() - tStart)/CLOCKS_PER_SEC));

  std::cout << "Write data = " << write_count.load() << std::endl;
  std::cout << "Read data = " << read_count.load() << std::endl;

  EXPECT_EQ(write_count, read_count);
}
