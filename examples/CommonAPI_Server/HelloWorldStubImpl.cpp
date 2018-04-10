//
// Created by redra on 01.05.17.
//

#include "HelloWorldStubImpl.hpp"

HelloWorldStubImpl::HelloWorldStubImpl()
 : icc::IComponent(nullptr) { }
HelloWorldStubImpl::~HelloWorldStubImpl() { }

void HelloWorldStubImpl::requestSayHello(const std::shared_ptr<CommonAPI::ClientId> _client,
                                         std::string const & _name, sayHelloReply_t _reply) {
  std::stringstream messageStream;
  messageStream << "Hello " << _name << "!";
  std::cout << "sayHello('" << _name << "'): '" << messageStream.str() << "'\n";
  _reply(messageStream.str());
};

void HelloWorldStubImpl::requestSetSettings(const std::shared_ptr<CommonAPI::ClientId> _client,
                                            std::vector< int32_t > const & _setting, setSettingsReply_t _reply) {
  for (auto num : _setting) {
    std::cout << "num = " << num << std::endl;
  }
  fireNewNameEvent("RedRa");
  _reply(13);
}
