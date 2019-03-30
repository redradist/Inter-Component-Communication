/**
 * @file CommandNotFound.hpp
 * @author Denis Kotov
 * @date 15 Mar 2018
 * @brief Contains exception for Not Found Command for ICC library
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef COMMONAPI_CLIENT_COMMANDNOTFOUND_HPP
#define COMMONAPI_CLIENT_COMMANDNOTFOUND_HPP

#include <icc/exceptions/ICCException.hpp>
#include <string>

namespace icc {

namespace command {

class CommandNotFound : public icc::ICCException {
 public:
  CommandNotFound(const char * _reason) {
    reason_ = _reason;
  }

  CommandNotFound(const std::string _reason) {
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

#endif //COMMONAPI_CLIENT_COMMANDNOTFOUND_HPP
