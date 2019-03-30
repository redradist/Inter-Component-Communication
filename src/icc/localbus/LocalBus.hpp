/**
 * @file LocalBus.hpp
 * @author Denis Kotov
 * @date 25 Jun 2017
 * @brief Contains LocalBus class.
 * It is a broker pattern. Used to control registration
 * and connection of services
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_LOCALBUS_HPP
#define ICC_LOCALBUS_HPP

#include <algorithm>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <icc/IComponent.hpp>
#include <icc/_private/helpers/hash_helpers.hpp>

namespace icc {

template<typename _T>
class Event;

template<typename _U, typename _T>
class Attribute;

namespace localbus {

template<typename _Interface>
class IClient;

template<typename _Interface>
class IService;

class LocalBus
    : public virtual IComponent {
 public:
  using tKeyForClientList = std::pair<std::type_index, std::string>;
  using tListOfClients = std::unordered_map<void*, std::function<void*(void)>>;
  using tKeyForServiceList = std::type_index;
  using tServiceStorage = std::function<void*(void)>;
  using tListOfServices = std::unordered_map<std::string, tServiceStorage>;

 public:
  /**
   * This function is used for getting LocalBus
   * It is used like Singleton pattern
   * @return LocalBus
   */
  static LocalBus &getBus();

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
    static_assert(std::is_abstract<_Interface>::value,
                  "_Interface is not an abstract class");
    invoke([=]() mutable {
      if (_service) {
        services_[tKeyForServiceList(typeid(_Interface))].
            emplace(_serviceName,
                    [_service]() mutable { return reinterpret_cast<void *>(&_service); });
        auto clientsKey = tKeyForClientList{typeid(_Interface), _serviceName};
        auto clients = clients_[clientsKey];
        for (auto client = clients.begin();
             client != clients.end();) {
          auto weak_client = *reinterpret_cast<std::weak_ptr<IClient<_Interface>>*>(client->second());
          if (auto _client = weak_client.lock()) {
            _client->push([=]() mutable {
              _client->connected(_service.get());
            });
            ++client;
          } else {
            // NOTE(redra): Deleting client
            client = clients.erase(client);
          }
        }
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
    static_assert(std::is_abstract<_Interface>::value,
                  "_Interface is not an abstract class");
    invoke([=] {
      if (_service) {
        services_[tKeyForServiceList(typeid(_Interface))].erase(_serviceName);
        auto clientsKey = tKeyForClientList{typeid(_Interface), _serviceName};
        auto clients = clients_[clientsKey];
        for (auto client = clients.begin();
             client != clients.end();) {
          auto weak_client = *reinterpret_cast<std::weak_ptr<IClient<_Interface>>*>(client->second());
          if (auto _client = weak_client.lock()) {
            _client->push([=]() mutable {
              _client->disconnected(nullptr);
            });
            ++client;
          } else {
            // NOTE(redra): Deleting client
            client = clients.erase(client);
          }
        }
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
  void buildClient(std::shared_ptr<IClient<_Interface>> _client,
                   const std::string &_serviceName) {
    static_assert(std::is_abstract<_Interface>::value,
                  "_Interface is not an abstract class");
    invoke([=] {
      if (_client) {
        auto clientsKey = tKeyForClientList{typeid(_Interface), _serviceName};
        std::weak_ptr<IClient<_Interface>> weak_client = _client;
        clients_[clientsKey].emplace(
            _client.get(),
            [weak_client]() mutable { return reinterpret_cast<void *>(&weak_client); });
        auto service = this->getService<_Interface>(_serviceName);
        if (service) {
          _client->invoke([=]() mutable {
            _client->connected(service.get());
          });
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
  void disassembleClient(IClient<_Interface> * _client,
                         const std::string &_serviceName) {
    static_assert(std::is_abstract<_Interface>::value,
                  "_Interface is not an abstract class");
    invoke([=] {
      if (_client) {
        auto clientsKey = tKeyForClientList{typeid(_Interface), _serviceName};
        clients_[clientsKey].erase(_client);
      }
    });
  }

 public:
  /**
   * This method is used to call method from IService<>
   * @tparam _Args Arguments types those is passed to external method
   * @tparam _Values Values types those is passed to external method
   * @param _callback Pointer to external method
   * @param _values Values passed into external method
   */
  template<typename _Interface,
           typename ... _Args,
           typename ... _Values>
  void call(const std::string &_serviceName,
            void(_Interface::*_callback)(_Args...), _Values ... _values) {
    static_assert(std::is_abstract<_Interface>::value,
                  "_Interface is not an abstract class");
    push([this, _serviceName, _callback, _values...]() mutable {
      auto service = this->getService<_Interface>(_serviceName);
      if (service) {
        service->invoke([=]() mutable {
          (service.get()->*_callback)(_values...);
        });
      }
    });
  };

  /**
   * This method is used to call method from IService<>
   * @tparam _Params Arguments types for reply
   * @tparam _Args Arguments types for external method
   * @tparam _Values Values types those is passed to external method
   * @param _callback Pointer to external method
   * @param _reply Reply with some result from IService<>
   * @param _values Arguments passed into external method
   */
  template<typename _Interface,
           typename ... _Params,
           typename ... _Args,
           typename ... _Values>
  void call(const std::string &_serviceName,
            void(_Interface::*_callback)(std::function<void(_Params...)>, _Args...),
            std::function<void(_Params...)> _reply,
            _Values ... _values) {
    static_assert(std::is_abstract<_Interface>::value,
                  "_Interface is not an abstract class");
    push([this, _serviceName, _callback, _reply, _values...]() mutable {
      auto service = this->getService<_Interface>(_serviceName);
      if (service) {
        std::function<void(_Params ...)> _safeReply =
        [=](_Params ... params) {
          push([=] {
            _reply(params...);
          });
        };
        service->invoke([=]() mutable {
          (service.get()->*_callback)(_safeReply, _values...);
        });
      }
    });
  };

  /**
   * This method is used to subscribe on event from IService<>
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Args Arguments of Event<>
   * @param _event Event for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename ... _Args>
  void subscribe(std::shared_ptr<_Client> _client,
                 const std::string & _serviceName,
                 Event<_R(_Args...)> _Interface::*_event,
                 _R(_Client::*_callback)(_Args...)) {
    static_assert(std::is_base_of<IComponent, _Client>::value,
                  "IComponent is not a base class of _Client");
    invoke([this, _client, _serviceName, _event, _callback] {
      auto service = this->getService<_Interface>(_serviceName);
      if (service) {
        service->invoke([=] {
          (service.get()->*_event).connect(_callback,
                                           _client);
        });
      }
    });
  };

  /**
   * This method is used to subscribe on event from IService<>
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Args Arguments of Event<>
   * @param _event Event for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename ... _Args>
  void subscribe(std::shared_ptr<IClient<_Interface>> _client,
                 const std::string & _serviceName,
                 Event<_R(_Args...)> _Interface::*_event,
                 _R(_Client::*_callback)(_Args...)) {
    static_assert(std::is_base_of<IClient<_Interface>, _Client>::value,
                  "IClient<_Interface> is not a base class of _Client");
    invoke([this, _client, _serviceName, _event, _callback] {
      auto service = this->getService<_Interface>(_serviceName);
      if (service) {
        service->invoke([=] {
          (service.get()->*_event).connect(
              _callback,
              std::static_pointer_cast<_Client>(_client));
        });
      }
    });
  };

  /**
   * This method is used to unsubscribe on event from IService<>
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Args Arguments of Event<>
   * @param _event Event for unsubscription
   * @param _callback Callback for unsubscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename ... _Args>
  void unsubscribe(std::shared_ptr<_Client> _client,
                   const std::string & _serviceName,
                   Event<_R(_Args...)> _Interface::*_event,
                   _R(_Client::*_callback)(_Args...)) {
    static_assert(std::is_base_of<IComponent, _Client>::value,
                  "IClient<_Interface> is not a base class of _Client");
    invoke([this, _client, _serviceName, _event, _callback] {
      auto service = this->getService<_Interface>(_serviceName);
      if (service) {
        service->invoke([=] {
          (service.get()->*_event).disconnect(_callback,
                                              _client);
        });
      }
    });
  };

  /**
   * This method is used to unsubscribe on event from IService<>
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Args Arguments of Event<>
   * @param _event Event for unsubscription
   * @param _callback Callback for unsubscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename ... _Args>
  void unsubscribe(std::shared_ptr<IClient<_Interface>> _client,
                   const std::string & _serviceName,
                   Event<_R(_Args...)> _Interface::*_event,
                   _R(_Client::*_callback)(_Args...)) {
    static_assert(std::is_base_of<IClient<_Interface>, _Client>::value,
                  "IClient<_Interface> is not a base class of _Client");
    invoke([this, _client, _serviceName, _event, _callback] {
      auto service = this->getService<_Interface>(_serviceName);
      if (service) {
        service->invoke([=] {
          (service.get()->*_event).disconnect(
              _callback,
              std::static_pointer_cast<_Client>(_client));
        });
      }
    });
  };

  /**
   * This method is used to subscribe on attribute from IService<>
   * @tparam _Client Client type in which located callback
   * @tparam _Arg Argument of Event<>
   * @param _attribute Attribute for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
      typename _Client,
      typename ... _Values,
      typename _Arg>
  void subscribe(std::shared_ptr<_Client> _client,
                 const std::string & _serviceName,
                 Attribute<_Values...> _Interface::*_attribute,
                 void(_Client::*_callback)(_Arg)) {
    static_assert(std::is_base_of<IComponent, _Client>::value,
                  "IComponent is not a base class of _Client");
    invoke([this, _client, _serviceName, _attribute, _callback] {
      auto service = this->getService<_Interface>(_serviceName);
      if (service) {
        service->invoke([=] {
          (service.get()->*_attribute).connect(_callback,
                                               _client);
          _client->push([=] {
            (_client.get()->*_callback)((service.get()->*_attribute).getValue());
          });
        });
      }
    });
  };

  /**
   * This method is used to subscribe on attribute from IService<>
   * @tparam _Client Client type in which located callback
   * @tparam _Arg Argument of Event<>
   * @param _attribute Attribute for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
      typename _Client,
      typename ... _Values,
      typename _Arg>
  void subscribe(std::shared_ptr<IClient<_Interface>> _client,
                 const std::string & _serviceName,
                 Attribute<_Values...> _Interface::*_attribute,
                 void(_Client::*_callback)(_Arg)) {
    static_assert(std::is_base_of<IClient<_Interface>, _Client>::value,
                  "IClient<_Interface> is not a base class of _Client");
    invoke([this, _client, _serviceName, _attribute, _callback] {
      auto service = this->getService<_Interface>(_serviceName);
      if (service) {
        service->invoke([=] {
          (service.get()->*_attribute).connect(
              _callback,
              std::static_pointer_cast<_Client>(_client));
          _client->push([=] {
            (_client.get()->*static_cast<void(IClient<_Interface>::*)(_Arg)>(_callback))((service.get()->*_attribute).getValue());
          });
        });
      }
    });
  };

  /**
   * This method is used to unsubscribe on attribute from IService<>
   * @tparam _Client Client type in which located callback
   * @tparam _Arg Argument of Event<>
   * @param _attribute Attribute for unsubscription
   * @param _callback Callback for unsubscription
   */
  template<typename _Interface,
           typename _Client,
           typename ... _Values,
           typename _Arg>
  void unsubscribe(std::shared_ptr<_Client> _client,
                   const std::string & _serviceName,
                   Attribute<_Values...> _Interface::*_attribute,
                   void(_Client::*_callback)(_Arg)) {
    static_assert(std::is_base_of<IComponent, _Client>::value,
                  "IClient<_Interface> is not a base class of _Client");
    invoke([this, _client, _serviceName, _attribute, _callback] {
      auto service = this->getService<_Interface>(_serviceName);
      if (service) {
        service->invoke([=] {
          (service.get()->*_attribute).disconnect(_callback,
                                                  _client);
        });
      }
    });
  };

  /**
   * This method is used to unsubscribe on attribute from IService<>
   * @tparam _Client Client type in which located callback
   * @tparam _Arg Argument of Event<>
   * @param _attribute Attribute for unsubscription
   * @param _callback Callback for unsubscription
   */
  template<typename _Interface,
           typename _Client,
           typename ... _Values,
           typename _Arg>
  void unsubscribe(std::shared_ptr<IClient<_Interface>> _client,
                   const std::string & _serviceName,
                   Attribute<_Values...> _Interface::*_attribute,
                   void(_Client::*_callback)(_Arg)) {
    static_assert(std::is_base_of<IClient<_Interface>, _Client>::value,
                  "IClient<_Interface> is not a base class of _Client");
    invoke([this, _client, _serviceName, _attribute, _callback] {
      auto service = this->getService<_Interface>(_serviceName);
      if (service) {
        service->invoke([=] {
          (service.get()->*_attribute).disconnect(
              _callback,
              std::static_pointer_cast<_Client>(_client));
        });
      }
    });
  };

 protected:
  /**
   * Helper function for finding service in LocalBus tables
   * @tparam _Interface Interface type for searching
   * @param _serviceName Service name for searching
   * @return Service std::shared_ptr<IService<_Interface>>
   */
  template<typename _Interface>
  std::shared_ptr<IService<_Interface>>
  getService(const std::string &_serviceName) const {
    auto servicesIter = services_.find(std::type_index(typeid(_Interface)));
    if (services_.end() != servicesIter) {
      auto serviceIter = std::find_if(servicesIter->second.begin(),
                                      servicesIter->second.end(),
                                      [=](std::pair<std::string, std::function<void *(void)>> element) {
                                        return element.first == _serviceName;
                                      });
      if (servicesIter->second.end() != serviceIter) {
        return *reinterpret_cast<std::shared_ptr<IService<_Interface>> *>(
                                 serviceIter->second());
      }
    }
    return nullptr;
  }

 private:
  LocalBus();
  LocalBus(const LocalBus &) = delete;
  LocalBus & operator=(const LocalBus &) = delete;
  LocalBus(LocalBus &&) = delete;
  LocalBus & operator=(LocalBus &&) = delete;
  virtual ~LocalBus();

 private:
  std::thread thread_;
  std::unordered_map<tKeyForClientList, tListOfClients, icc::helpers::pair_hash> clients_;
  std::unordered_map<tKeyForServiceList, tListOfServices> services_;
};

}

}

#endif //ICC_LOCALBUS_HPP
