//
// Created by redra on 01.05.17.
//

#ifndef COMMONAPI_SERVER_HELLOWORLDSTUBIMPL_HPP
#define COMMONAPI_SERVER_HELLOWORLDSTUBIMPL_HPP

#include <CommonAPI/DBus/CommonAPIDBus.hpp>
#include <CommonAPI/CommonAPI.hpp>
#include "HelloWorldService.hpp"

class HelloWorldStubImpl
    : public icc::commonapi::HelloWorldService<> {
 public:
  HelloWorldStubImpl();
  virtual ~HelloWorldStubImpl();
  void requestSayHello(const std::shared_ptr<CommonAPI::ClientId> _client,
                       std::string  const & _name,
                       sayHelloReply_t _return) override;
  void requestSetSettings(const std::shared_ptr<CommonAPI::ClientId> _client,
                          std::vector< int32_t >  const & _setting,
                          setSettingsReply_t _reply) override;
};

#endif //COMMONAPI_SERVER_HELLOWORLDSTUBIMPL_HPP
