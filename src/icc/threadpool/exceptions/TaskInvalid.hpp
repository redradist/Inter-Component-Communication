/**
 * @file TaskInvalid.hpp
 * @author Denis Kotov
 * @date 09 Apr 2018
 * @brief Throws when task is invalid
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_TREADPOOL_TASKINVALID_HPP
#define ICC_TREADPOOL_TASKINVALID_HPP

#include <icc/exceptions/ICCException.hpp>
#include <string>

namespace icc {

namespace pools {

class TaskInvalid : public icc::ICCException {
 public:
  TaskInvalid(const char * _reason) {
    reason_ = _reason;
  }

  TaskInvalid(const std::string _reason) {
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

#endif //ICC_TREADPOOL_TASKINVALID_HPP
