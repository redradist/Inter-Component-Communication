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
#include <boost/asio/io_service.hpp>
#include <icc/Component.hpp>

namespace icc {

namespace pools {

class ThreadPool {
  struct IoServiceMetaData {
    std::shared_ptr<boost::asio::io_service::work> worker_;
    size_t number_of_tasks_ = 0;

    IoServiceMetaData(std::shared_ptr<boost::asio::io_service> _service) {
      worker_ = std::make_shared<boost::asio::io_service::work>(*_service);
    }
  };

  struct IoServicesComparator {
    bool operator()(const IoServiceMetaData& _first,
                    const IoServiceMetaData& _second) const {
      return _first.number_of_tasks_ > _second.number_of_tasks_;
    }
  };

 public:
  ThreadPool(const unsigned int _numThreads);
  ~ThreadPool();

  static ThreadPool & getPool(
      const unsigned int _numThreads = std::thread::hardware_concurrency());

  /**
   * Method used to push task for execution
   * @param _task Task that will be executed
   */
  void push(std::function<void(void)> _task);

 protected:
  std::vector<std::thread> thread_pool_;
  std::vector<std::shared_ptr<boost::asio::io_service>> services_;
  std::priority_queue<IoServiceMetaData,
                      std::vector<IoServiceMetaData>,
                      IoServicesComparator> services_meta_data_;
};

}

}

#endif //ICC_THREADPOOL_HPP
