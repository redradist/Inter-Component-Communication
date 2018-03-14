//
// Created by redra on 15.03.18.
//

#ifndef COMMONAPI_CLIENT_COMMANDNOTFOUND_HPP
#define COMMONAPI_CLIENT_COMMANDNOTFOUND_HPP

#include <exceptions/ICCException.hpp>
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
