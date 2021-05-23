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
        while (!task_queue_.isInterrupt()) {
          Action task = task_queue_.waitPop();
          if (task) {
            task();
          }
        }
      });
    }
  } catch (...) {
    stop();
    throw;
  }
}

ThreadPool::ThreadPool(const ThreadAction& initThreadTask,
                       const unsigned _numThreads) {
  try {
    ThreadLoop threadLoop = [this] {
      while (!task_queue_.isInterrupt()) {
        Action task = task_queue_.waitPop();
        if (task) {
          task();
        }
      }
    };
    for (int i = 0; i < _numThreads; ++i) {
      threads_.emplace_back([initThreadTask, threadLoop] {
        initThreadTask(threadLoop);
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
  task_queue_.interrupt();
  threads_.clear();
}

ThreadPool&
ThreadPool::getDefaultPool(const unsigned _numThreads) {
  static ThreadPool pool(_numThreads);
  return pool;
}

std::shared_ptr<ThreadPool>
ThreadPool::createPool(const unsigned _numThreads) {
  return std::shared_ptr<ThreadPool>(new ThreadPool(_numThreads));
}

std::shared_ptr<ThreadPool>
ThreadPool::createCustomPool(const ThreadAction& threadTask,
                             const unsigned _numThreads) {
  return std::shared_ptr<ThreadPool>(new ThreadPool(threadTask, _numThreads));
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
