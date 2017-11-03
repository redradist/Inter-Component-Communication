//
// Created by redra on 01.05.17.
//

#include "HelloWorldStubImpl.hpp"

HelloWorldStubImpl::HelloWorldStubImpl() { }
HelloWorldStubImpl::~HelloWorldStubImpl() { }

void HelloWorldStubImpl::sayHello(const std::shared_ptr<CommonAPI::ClientId> _client,
                                  std::string _name, sayHelloReply_t _reply) {
  std::stringstream messageStream;
  messageStream << "Hello " << _name << "!";
  std::cout << "sayHello('" << _name << "'): '" << messageStream.str() << "'\n";
  _reply(messageStream.str());
};

void HelloWorldStubImpl::setSettings(const std::shared_ptr<CommonAPI::ClientId> _client, std::vector< int32_t > _setting, setSettingsReply_t _reply) {
  for (auto num : _setting) {
    std::cout << "num = " << num << std::endl;
  }
  fireNewNameEvent("RedRa");
  _reply(13);
}
