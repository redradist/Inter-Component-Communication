/**
 * @file ThreadPool.cpp
 * @author Denis Kotov
 * @date 08 Apr 2018
 * @brief Implementation of ThreadPool support
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#include <thread>
#include <utility>

#include "ThreadPool.hpp"
#include "Task.hpp"

namespace icc {

namespace threadpool {

ThreadPool::ThreadPool(const unsigned _numThreads) {
  try {
    for (int i = 0; i < _numThreads; ++i) {
      threads_.emplace_back([this] {
        do {
          Action task = task_queue_.waitPop();
          if (task) {
            task();
          }
        } while (execute_.load(std::memory_order_acquire));
      });
    }
  } catch (...) {
    stop();
    throw;
  }
}

ThreadPool::~ThreadPool() {
  stop();
}

void ThreadPool::stop() {
  execute_.store(false, std::memory_order_release);
  task_queue_.interrupt();
  threads_.clear();
}

ThreadPool &
ThreadPool::getDefaultPool(const unsigned _numThreads) {
  static ThreadPool pool(_numThreads);
  return pool;
}

std::shared_ptr<ThreadPool>
ThreadPool::getPool(const unsigned _numThreads) {
  return std::shared_ptr<ThreadPool>(new ThreadPool(_numThreads));
}

void ThreadPool::push(Action _task) {
  task_queue_.push(std::move(_task));
}

bool ThreadPool::hasThread(std::thread::id _threadId) const {
  return std::any_of(threads_.begin(), threads_.end(),
  [&_threadId](const JThread & _thread) {
    return _thread.getId() == _threadId;
  });
}

}

}
