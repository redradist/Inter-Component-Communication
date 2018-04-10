/**
 * @file TaskStateAssert.hpp
 * @author Denis Kotov
 * @date 09 Apr 2018
 * @brief Throws when task has whether invalid state or
 * to be tried to change to new state from invalid state
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_TREADPOOL_TASKSTATEASSERT_HPP
#define ICC_TREADPOOL_TASKSTATEASSERT_HPP

#include <exceptions/ICCException.hpp>
#include <string>

namespace icc {

namespace pools {

class TaskStateAssert : public icc::ICCException {
 public:
  TaskStateAssert(const char * _reason) {
    reason_ = _reason;
  }

  TaskStateAssert(const std::string _reason) {
    reason_ = _reason;
  }

  virtual const char *what() const noexcept override {
    return reason_.c_str();
  }

 private:
  std::string reason_;
};

}

}

#endif //ICC_TREADPOOL_TASKSTATEASSERT_HPP
