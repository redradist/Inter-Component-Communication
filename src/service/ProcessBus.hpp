//
// Created by redra on 25.06.17.
//

#ifndef ICC_PROCESSBUS_HPP
#define ICC_PROCESSBUS_HPP

#include <typeinfo>
#include <set>
#include <map>
#include <algorithm>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include "../IComponent.hpp"

template <typename _Interface>
class IClient;

template <typename _Interface>
class IService;

class ProcessBus
  : public IComponent {
 public:
  using tNameAndService = std::pair<std::string, void*>;
  using tListOfService = std::vector<tNameAndService>;

  template <typename _Interface>
  void registerService(IService<_Interface> * _service,
                       const std::string & _serviceName) {
    push([=]{
      mapp_[std::type_index(typeid(_Interface))].emplace_back(_serviceName, _service);
    });
  }

  template <typename _Interface>
  void buildClient(IClient<_Interface> * _client,
                   const std::string & _serviceName) {
    push([=] {
      auto servicesIter = mapp_.find(std::type_index(typeid(_Interface)));
      if (mapp_.end() != servicesIter) {
        auto serviceIter = std::find_if(servicesIter->second.begin(),
                                        servicesIter->second.end(),
        [=](std::pair<std::string, void*> element) {
          return element.first == _serviceName;
        });
        if (serviceIter != servicesIter->second.end()) {
          _client->push([=]{
            _client->setService(
                reinterpret_cast<_Interface*>(serviceIter->second));
          });
        }
      }
    });
  }

 public:
  static ProcessBus & getBus();

 private:
  ProcessBus() = default;
  ProcessBus(const ProcessBus &) = delete;
  ProcessBus(ProcessBus &&) = delete;

 private:
  std::unordered_map<std::type_index, tListOfService> mapp_;
};

#endif //ICC_PROCESSBUS_HPP
