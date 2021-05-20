//
// Created by RedRa on 20.05.2021.
//

#ifndef ICC_SRC_ICC_THREADPOOL_JTHREAD_HPP_
#define ICC_SRC_ICC_THREADPOOL_JTHREAD_HPP_

#include <thread>

namespace icc {

namespace threadpool {

class JThread final {
 public:
  template<typename Function, typename ... Args>
  explicit JThread(Function&& f, Args&&... args) {
    thread_ = std::thread(
        std::forward<Function>(f),
        std::forward<Args>(args)...);
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
