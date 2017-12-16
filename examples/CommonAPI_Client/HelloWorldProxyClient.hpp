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
  void debug(const std::string & str) {

  }
  void warning(const std::string & str) {

  }
  void error(const std::string & str) {

  }
};

class HelloWorldProxyClient
  : public icc::commonapi::HelloWorldClient<NewLogger> {
 public:
  HelloWorldProxyClient(const std::string &_domain,
                        const std::string &_instance);
  virtual ~HelloWorldProxyClient();

  virtual void connected(v1::commonapi::HelloWorldProxy<> &);
  virtual void disconnected(v1::commonapi::HelloWorldProxy<> &);
};

#endif //COMMONAPI_SERVER_HELLOWORLDSTUBIMPL_HPP
