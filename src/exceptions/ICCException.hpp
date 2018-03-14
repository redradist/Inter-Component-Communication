//
// Created by redra on 15.03.18.
//

#ifndef COMMONAPI_CLIENT_ICCEXCEPTION_HPP
#define COMMONAPI_CLIENT_ICCEXCEPTION_HPP

#include <exception>

namespace icc {

class ICCException : public std::exception {
 public:
  virtual const char* what() const noexcept override {
    return "Exception from ICC library";
  }
};

}

#endif //COMMONAPI_CLIENT_ICCEXCEPTION_HPP
