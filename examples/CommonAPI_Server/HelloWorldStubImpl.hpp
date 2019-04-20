//
// Created by redra on 01.05.17.
//

#ifndef COMMONAPI_SERVER_HELLOWORLDSTUBIMPL_HPP
#define COMMONAPI_SERVER_HELLOWORLDSTUBIMPL_HPP

#include <CommonAPI/DBus/CommonAPIDBus.hpp>
#include <CommonAPI/CommonAPI.hpp>
#include <icc/Component.hpp>
#include <icc/coroutine/Task.hpp>
#include "HelloWorldService.hpp"
#include "HelloWorldClient.hpp"

class DummyLogger2 {
 public:
  inline void debug(const std::string & _str) {
    // Dummy implementation
  }

  inline void info(const std::string & _str) {
    // Dummy implementation
  }

  inline void warning(const std::string & _str) {
    // Dummy implementation
  }

  inline void error(const std::string & _str) {
    // Dummy implementation
  }
};

class HelloWorldStubImpl
    : public icc::commonapi::HelloWorldService<>
    , public icc::commonapi::HelloWorldClient<DummyLogger2> {
 public:
  HelloWorldStubImpl();
  virtual ~HelloWorldStubImpl();
  void requestSayHello(const std::shared_ptr<CommonAPI::ClientId> _client,
                       std::string  const & _name,
                       sayHelloReply_t _return) override;
//  icc::coroutine::Task<void>
//  requestSayHelloCoro(const std::shared_ptr<CommonAPI::ClientId> _client,
//      std::string const & _name,
//      sayHelloReply_t _reply) override;
  void requestSetSettings(const std::shared_ptr<CommonAPI::ClientId> _client,
                          std::vector< int32_t >  const & _setting,
                          setSettingsReply_t _reply) override;
};

#endif //COMMONAPI_SERVER_HELLOWORLDSTUBIMPL_HPP
