/**
 * @file ProcessBus.hpp
 * @author Denis Kotov
 * @date 25 Jun 2017
 * @brief Contains ProcessBus class.
 * It is a broker pattern. Used to control registration
 * and connection of services
 * @copyright Denis Kotov, MIT License. Open source:
 */

#ifndef ICC_PROCESSBUS_HPP
#define ICC_PROCESSBUS_HPP

#include <set>
#include <map>
#include <algorithm>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <IComponent.hpp>

template <typename _Interface>
class IClient;

template <typename _Interface>
class IService;

class ProcessBus
  : public IComponent {
 public:
  using tKeyForClientList = std::pair<std::type_index, std::string>;
  using tListOfClients = std::set<void*>;
  using tKeyForServiceList = std::type_index;
  using tListOfServices = std::map<std::string, void*>;

 public:
  template <typename _Interface>
  void registerService(IService<_Interface> * _service,
                       const std::string & _serviceName) {
    push([=]{
      services_[tKeyForServiceList(typeid(_Interface))].emplace(_serviceName, _service);
      auto clientsKey = tKeyForClientList{typeid(_Interface), _serviceName};
      auto clients = clients_[clientsKey];
      for (auto client : clients) {
        reinterpret_cast<IClient<_Interface>*>(client)->setService(_service);
      }
    });
  }

  template <typename _Interface>
  void unregisterService(IService<_Interface> * _service,
                         const std::string & _serviceName) {
    push([=]{
      auto clientsKey = tKeyForClientList{typeid(_Interface), _serviceName};
      auto clients = clients_[clientsKey];
      for (auto client : clients) {
        reinterpret_cast<IClient<_Interface>*>(client)->setService(nullptr);
      }
    });
  }

  template <typename _Interface>
  void buildClient(IClient<_Interface> * _client,
                   const std::string & _serviceName) {
    push([=] {
      auto clientsKey = tKeyForClientList{typeid(_Interface), _serviceName};
      clients_[clientsKey].emplace(_client);
      auto servicesIter = services_.find(std::type_index(typeid(_Interface)));
      if (services_.end() != servicesIter) {
        auto serviceIter = std::find_if(servicesIter->second.begin(),
                                        servicesIter->second.end(),
        [=](std::pair<std::string, void*> element) {
          return element.first == _serviceName;
        });
        if (servicesIter->second.end() != serviceIter) {
          _client->setService(
              reinterpret_cast<IService<_Interface>*>(serviceIter->second));
        }
      }
    });
  }

  template <typename _Interface>
  void disassembleClient(IClient<_Interface> * _client,
                         const std::string & _serviceName) {
    push([=] {
      auto clientsKey = tKeyForClientList{typeid(_Interface), _serviceName};
      clients_[clientsKey].erase(_client);
    });
  }

 public:
  static ProcessBus & getBus();

 private:
  ProcessBus();
  ProcessBus(const ProcessBus &) = delete;
  ProcessBus(ProcessBus &&) = delete;
  virtual ~ProcessBus();

 private:
  std::thread thread_;
  std::map<tKeyForClientList, tListOfClients> clients_;
  std::map<tKeyForServiceList, tListOfServices> services_;
};

#endif //ICC_PROCESSBUS_HPP
