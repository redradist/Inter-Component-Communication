//
// Created by redra on 01.05.17.
//

#include "HelloWorldProxyClient.hpp"

HelloWorldProxyClient::HelloWorldProxyClient(const std::string &_domain,
                                             const std::string &_instance)
    : HelloWorldClient<NewLogger>(_domain, _instance)
    , NewLogger("") { }
HelloWorldProxyClient::~HelloWorldProxyClient() { }

void HelloWorldProxyClient::connected(v1::commonapi::HelloWorldProxy<> &) {
  std::cout << "v1::commonapi::HelloWorldProxy is connected" << std::endl;
  debug("v1::commonapi::HelloWorldProxy is connected");
}

void HelloWorldProxyClient::disconnected(v1::commonapi::HelloWorldProxy<> &) {
  std::cout << "v1::commonapi::HelloWorldProxy is disconnected" << std::endl;
}