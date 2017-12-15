//
// Created by redra on 01.05.17.
//

#ifndef COMMONAPI_SERVER_HELLOWORLDSTUBIMPL_HPP
#define COMMONAPI_SERVER_HELLOWORLDSTUBIMPL_HPP

#include <CommonAPI/DBus/CommonAPIDBus.hpp>
#include <CommonAPI/CommonAPI.hpp>
#include <v1/commonapi/HelloWorldProxy.hpp>
#include <commonapi/CommonAPIClient.hpp>
#include <HelloWorldClient.hpp>

class NewLogger {
 public:
  std::string sadsdasf;
  NewLogger() {
    std::cout << "NewLogger()" << std::endl;
  }
  NewLogger(const std::string & _str) {
    std::cout << "NewLogger(const std::string & _str)" << std::endl;
  }

  NewLogger(NewLogger && _str) {
    std::cout << "NewLogger(NewLogger && _str)" << std::endl;
  }

 protected:
  virtual void myFunction() {
    std::cout << sadsdasf << std::endl;
  }

  inline void debug(const std::string & _str) {
    std::cout << _str << std::endl;
    myFunction();
  }

  inline void info(const std::string & _str) {
    std::cout << _str << std::endl;
  }

  inline void warning(const std::string & _str) {
    std::cout << _str << std::endl;
  }

  inline void error(const std::string & _str) {
    std::cout << _str << std::endl;
  }
};

class HelloWorldProxyClient
  : public icc::commonapi::HelloWorldClient<NewLogger>
  , public virtual NewLogger {
 public:
  HelloWorldProxyClient(const std::string &_domain,
                        const std::string &_instance);
  virtual ~HelloWorldProxyClient();

  virtual void connected(v1::commonapi::HelloWorldProxy<> &);
  virtual void disconnected(v1::commonapi::HelloWorldProxy<> &);
};

#endif //COMMONAPI_SERVER_HELLOWORLDSTUBIMPL_HPP
