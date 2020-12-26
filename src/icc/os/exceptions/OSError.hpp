/**
 * @file OSError.hpp
 * @author Denis Kotov
 * @date 26 Dec 2020
 * @brief Contains exception for OS part of ICC library
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_OS_POSIX_EXCEPTIONS_OSERROR_HPP
#define ICC_OS_POSIX_EXCEPTIONS_OSERROR_HPP

#include <string>
#include <icc/exceptions/ICCException.hpp>

namespace icc {

namespace os {

class OSError : public icc::ICCException {
 public:
  OSError(const char * _reason) {
    reason_ = _reason;
  }

  OSError(const std::string _reason) {
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

#endif //ICC_OS_POSIX_EXCEPTIONS_OSERROR_HPP
