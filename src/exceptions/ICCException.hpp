/**
 * @file ICCException.hpp
 * @author Denis Kotov
 * @date 15 Mar 2018
 * @brief Contains base class exception for ICC library
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

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
