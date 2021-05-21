/**
 * @file ThreadPool.hpp
 * @author Denis Kotov
 * @date 08 Apr 2018
 * @brief Definition of ThreadPool library
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_THREADPOOL_HPP
#define ICC_THREADPOOL_HPP

#include <queue>
#include <mutex>

#include <icc/Component.hpp>
#include <icc/_private/helpers/memory_helpers.hpp>
#include "JThread.hpp"

namespace icc {

namespace threadpool {

using Action = std::function<void(void)>;
using ThreadSafeQueueAction = icc::_private::containers::ThreadSafeQueue<Action>;

template <typename T>
class Task;

class ThreadPool
    : public icc::helpers::virtual_enable_shared_from_this< ThreadPool > {
 public:
  ~ThreadPool();

  static ThreadPool & getDefaultPool(
      unsigned _numThreads = std::thread::hardware_concurrency());

  static std::shared_ptr<ThreadPool> getPool(
      unsigned _numThreads = std::thread::hardware_concurrency());

  template<typename TRes>
  Task<TRes> createTask(std::function<TRes(void)> _task) {
    return Task<TRes>(std::move(_task)).setThreadPool(shared_from_this());
  }

  /**
   * Method used to push task for execution
   * @param _task Task that will be executed
   */
  void push(Action _task);

  /**
   * Method used to check if thread with id _threadId
   * is owned by this ThreadPool
   * @param _threadId Thread Id to check
   * @return true if thread belongs to, false otherwise
   */
  bool hasThread(std::thread::id _threadId) const;

 protected:
  explicit ThreadPool(unsigned _numThreads);
  void stop();

  std::vector<JThread> threads_;
  std::atomic_bool execute_{true};
  ThreadSafeQueueAction task_queue_;
};

}

}

#endif //ICC_THREADPOOL_HPP
