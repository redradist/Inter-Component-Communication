/**
 * @file ThreadPool.cpp
 * @author Denis Kotov
 * @date 08 Apr 2018
 * @brief Implementation of ThreadPool support
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#include <thread>
#include "Task.hpp"
#include "ThreadPool.hpp"

namespace icc {

namespace pools {

ThreadPool::ThreadPool(const unsigned int _numThreads) {
  for (int i = 0; i < _numThreads; ++i) {
    services_.emplace_back(new boost::asio::io_service());
    services_meta_data_.emplace(services_[i]);
    thread_pool_.emplace_back([=] {
      services_[i]->run();
    });
  }
}

ThreadPool::~ThreadPool() {
  while (!services_meta_data_.empty()) {
    services_meta_data_.pop();
  }

  for (auto & thread : thread_pool_) {
    thread.join();
  }
}

ThreadPool &
ThreadPool::getPool(const unsigned int _numThreads) {
  static ThreadPool pool(_numThreads);
  return pool;
}

void ThreadPool::push(std::function<void(void)> _task) {
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  auto serviceMetaData = services_meta_data_.top();
  services_meta_data_.pop();
  serviceMetaData.number_of_tasks_++;
  services_meta_data_.push(serviceMetaData);
  serviceMetaData.worker_->get_io_service().post([=] () mutable {
    _task();
    {
      std::lock_guard<std::recursive_mutex> lock(mutex_);
      auto serviceMetaData = std::move(services_meta_data_.top());
      services_meta_data_.pop();
      serviceMetaData.number_of_tasks_--;
      services_meta_data_.push(std::move(serviceMetaData));
    }
  });
}

}

}
