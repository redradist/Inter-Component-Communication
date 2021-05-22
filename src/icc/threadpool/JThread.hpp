/**
 * @file Attribute.hpp
 * @author Denis Kotov
 * @date 20 May 2021
 * @brief Contains JThread class.
 * It is an thread class that joins in destructor
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_SRC_ICC_THREADPOOL_JTHREAD_HPP_
#define ICC_SRC_ICC_THREADPOOL_JTHREAD_HPP_

#include <thread>

namespace icc {

namespace threadpool {

class JThread final {
 public:
  template<typename TFunction, typename ... TArgs>
  explicit JThread(TFunction&& f, TArgs&&... args) {
    thread_ = std::thread(
        std::forward<TFunction>(f),
        std::forward<TArgs>(args)...);
  }
  JThread(JThread&&) noexcept = default;
  JThread& operator=(JThread&&) noexcept = default;
  JThread(const JThread&) noexcept = delete;
  JThread& operator=(const JThread&) noexcept = delete;

  ~JThread() {
    if (thread_.joinable()) {
      thread_.join();
    }
  }

  std::thread::id getId() const noexcept {
    return thread_.get_id();
  }

 private:
  std::thread thread_;
};

}

}

#endif //ICC_SRC_ICC_THREADPOOL_JTHREAD_HPP_
