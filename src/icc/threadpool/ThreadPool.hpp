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
#include <icc/_private/api.hpp>
#include "JThread.hpp"

namespace icc {

namespace threadpool {

using Action = std::function<void(void)>;
using ThreadSafeActionQueue = icc::_private::containers::ThreadSafeQueue<Action>;

template <typename T>
class Task;

class ICC_PUBLIC ThreadPool
    : public icc::helpers::virtual_enable_shared_from_this< ThreadPool > {
 public:
  using ThreadLoop = std::function<void(void)>;
  using ThreadAction = std::function<void(const ThreadLoop&)>;

  ~ThreadPool() override;

  static ThreadPool & getDefaultPool(
      unsigned _numThreads = std::thread::hardware_concurrency());

  static std::shared_ptr<ThreadPool> createPool(
      unsigned _numThreads = std::thread::hardware_concurrency());

  static std::shared_ptr<ThreadPool> createCustomPool(
      const ThreadAction& threadTask,
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
  explicit ThreadPool(const ThreadAction& initThreadTask, unsigned _numThreads);
  /**
   * Method that stop ThreadPool
   */
  void stop();

  ThreadSafeActionQueue task_queue_;
  std::vector<JThread> threads_;
};

}

}

#endif //ICC_THREADPOOL_HPP
