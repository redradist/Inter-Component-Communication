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

namespace icc {

namespace service {

template<typename _Interface>
class IClient;

template<typename _Interface>
class IService;

class ProcessBus
    : public virtual IComponent {
 public:
  using tKeyForClientList = std::pair<std::type_index, std::string>;
  using tListOfClients = std::set<void *>;
  using tKeyForServiceList = std::type_index;
  using tServiceStorage = std::function<void *(void)>;
  using tListOfServices = std::map<std::string, tServiceStorage>;

 public:
  /**
   * This function is used for getting ProcessBus
   * It is used like Singleton pattern
   * @return ProcessBus
   */
  static ProcessBus &getBus();

 public:
  /**
   * This method is used for registering service
   * @tparam _Interface Interface for registration
   * @param _service Object of service for registration
   * @param _serviceName Name of service for registration
   */
  template<typename _Interface>
  void registerService(std::shared_ptr<IService<_Interface>> _service,
                       const std::string &_serviceName) {
    push([=]() mutable {
      services_[tKeyForServiceList(typeid(_Interface))].
          emplace(_serviceName,
                  [_service]() mutable { return reinterpret_cast<void *>(&_service); });
      auto clientsKey = tKeyForClientList{typeid(_Interface), _serviceName};
      auto clients = clients_[clientsKey];
      for (auto client : clients) {
        reinterpret_cast<IClient<_Interface> *>(client)->setService(_service);
      }
    });
  }

  /**
   * This method is used for unregistering service
   * @tparam _Interface Interface for unregistration
   * @param _service Object of service for unregistration
   * @param _serviceName Name of service for unregistration
   */
  template<typename _Interface>
  void unregisterService(std::shared_ptr<IService<_Interface>> _service,
                         const std::string &_serviceName) {
    push([=] {
      services_[tKeyForServiceList(typeid(_Interface))].erase(_serviceName);
      auto clientsKey = tKeyForClientList{typeid(_Interface), _serviceName};
      auto clients = clients_[clientsKey];
      for (auto client : clients) {
        reinterpret_cast<IClient<_Interface> *>(client)->setService(nullptr);
      }
    });
  }

  /**
   * This method is used for building client
   * (IClient<_Interface>) of IService<_Interface>
   * @tparam _Interface Interface for building client
   * @param _client Object of client waiting for building
   * @param _serviceName Name of service for building
   */
  template<typename _Interface>
  void buildClient(IClient<_Interface> *_client,
                   const std::string &_serviceName) {
    push([=] {
      auto clientsKey = tKeyForClientList{typeid(_Interface), _serviceName};
      clients_[clientsKey].emplace(_client);
      auto servicesIter = services_.find(std::type_index(typeid(_Interface)));
      if (services_.end() != servicesIter) {
        auto serviceIter = std::find_if(servicesIter->second.begin(),
                                        servicesIter->second.end(),
                                        [=](std::pair<std::string, std::function<void *(void)>> element) {
                                          return element.first == _serviceName;
                                        });
        if (servicesIter->second.end() != serviceIter) {
          _client->setService(
              *reinterpret_cast<std::shared_ptr<IService<_Interface>> *>(
                  serviceIter->second()));
        }
      }
    });
  }

  /**
   * This method is used for disassembling client
   * (IClient<_Interface>) of IService<_Interface>
   * @tparam _Interface Interface for building client
   * @param _client Object of client waiting for building
   * @param _serviceName Name of service for building
   */
  template<typename _Interface>
  void disassembleClient(IClient<_Interface> *_client,
                         const std::string &_serviceName) {
    push([=] {
      auto clientsKey = tKeyForClientList{typeid(_Interface), _serviceName};
      clients_[clientsKey].erase(_client);
    });
  }

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

}

}

#endif //ICC_PROCESSBUS_HPP
