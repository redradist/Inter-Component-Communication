/**
 * @file ContainerError.hpp
 * @author Denis Kotov
 * @date 15 Mar 2018
 * @brief Contains exception for errors in containers of ICC library
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_CONTAINER_ERROR_HPP
#define ICC_CONTAINER_ERROR_HPP

#include <string>
#include <icc/exceptions/ICCException.hpp>

namespace icc {

namespace _private {

namespace containers {

class ContainerError : public icc::ICCException {
 public:
  ContainerError(const char *_reason) {
    reason_ = _reason;
  }

  ContainerError(const std::string _reason) {
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

}

#endif //ICC_CONTAINER_ERROR_HPP
