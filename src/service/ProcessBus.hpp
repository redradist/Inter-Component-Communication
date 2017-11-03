/**
 * @file ProcessBus.hpp
 * @author Denis Kotov
 * @date 25 Jun 2017
 * @brief Contains ProcessBus class.
 * It is a broker pattern. Used to control registration
 * and connection of services
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_PROCESSBUS_HPP
#define ICC_PROCESSBUS_HPP

#include <map>
#include <algorithm>
#include <typeinfo>
#include <typeindex>
#include <IComponent.hpp>

namespace icc {

template<typename _T>
class Event;

namespace service {

template<typename _Interface>
class IClient;

template<typename _Interface>
class IService;

class ProcessBus
    : public virtual IComponent {
 public:
  using tKeyForClientList = std::pair<std::type_index, std::string>;
  using tListOfClients = std::map<void*, std::function<void*(void)>>;
  using tKeyForServiceList = std::type_index;
  using tServiceStorage = std::function<void*(void)>;
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
            _client->connected(_service.get());
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
            _client->disconnected(nullptr);
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
          _client->connected(service.get());
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
   * @tparam _Interface Interface for calling method
   * @param _serviceName Service name to send request
   * @param _callback Pointer to external method without parameters
   */
  template<typename _Interface>
  void call(const std::string &_serviceName,
            void(_Interface::*_callback)(void)) {
    static_assert(std::is_abstract<_Interface>::value,
                  "_Interface is not an abstract class");
    push([=]() mutable {
      auto service = this->getService<_Interface>(_serviceName);
      if (service) {
        service->invoke([=]() mutable {
          (service.get()->*_callback)();
        });
      }
    });
  };

  /**
   * This method is used to call method from IService<>
   * @tparam _Interface Interface for calling method
   * @tparam _Arg0 Argument #0 type those is passed to external method
   * @param _serviceName Service name to send request
   * @param _callback Pointer to external method
   * @param _value0 Argument #0 passed into external method
   */
  template<typename _Interface,
           typename _Arg0,
           typename _Value0>
  void call(const std::string &_serviceName,
            void(_Interface::*_callback)(_Arg0),
            _Value0 && _value0) {
    static_assert(std::is_abstract<_Interface>::value,
                  "_Interface is not an abstract class");
    push([=]() mutable {
      auto service = this->getService<_Interface>(_serviceName);
      if (service) {
        service->invoke([=]() mutable {
          (service.get()->*_callback)(_value0);
        });
      }
    });
  };

  /**
   * This method is used to call method from IService<>
   * @tparam _Interface Interface for calling method
   * @tparam _Arg0 Argument #0 type those is passed to external method
   * @tparam _Arg1 Argument #1 type those is passed to external method
   * @param _serviceName Service name to send request
   * @param _callback Pointer to external method
   * @param _value0 Argument #0 passed into external method
   * @param _value1 Argument #1 passed into external method
   */
  template<typename _Interface,
           typename _Arg0,
           typename _Arg1,
           typename _Value0,
           typename _Value1>
  void call(const std::string &_serviceName,
            void(_Interface::*_callback)(_Arg0, _Arg1),
            _Value0 && _value0, _Value1 && _value1) {
    static_assert(std::is_abstract<_Interface>::value,
                  "_Interface is not an abstract class");
    push([=]() mutable {
      auto service = this->getService<_Interface>(_serviceName);
      if (service) {
        service->invoke([=]() mutable {
          (service.get()->*_callback)(_value0, _value1);
        });
      }
    });
  };

  /**
   * This method is used to call method from IService<>
   * @tparam _Interface Interface for calling method
   * @tparam _Arg0 Argument #0 type those is passed to external method
   * @tparam _Arg1 Argument #1 type those is passed to external method
   * @tparam _Arg2 Argument #2 type those is passed to external method
   * @param _serviceName Service name to send request
   * @param _callback Pointer to external method
   * @param _value0 Argument #0 passed into external method
   * @param _value1 Argument #1 passed into external method
   * @param _value2 Argument #2 passed into external method
   */
  template<typename _Interface,
           typename _Arg0,
           typename _Arg1,
           typename _Arg2,
           typename _Value0,
           typename _Value1,
           typename _Value2>
  void call(const std::string &_serviceName,
            void(_Interface::*_callback)(_Arg0, _Arg1, _Arg2),
            _Value0 && _value0, _Value1 && _value1, _Value2 && _value2) {
    static_assert(std::is_abstract<_Interface>::value,
                  "_Interface is not an abstract class");
    push([=]() mutable {
      auto service = this->getService<_Interface>(_serviceName);
      if (service) {
        service->invoke([=]() mutable {
          (service.get()->*_callback)(_value0, _value1, _value2);
        });
      }
    });
  };

  /**
   * This method is used to call method from IService<>
   * @tparam _Interface Interface for calling method
   * @tparam _Arg0 Argument #0 type those is passed to external method
   * @tparam _Arg1 Argument #1 type those is passed to external method
   * @tparam _Arg2 Argument #2 type those is passed to external method
   * @tparam _Arg3 Argument #3 type those is passed to external method
   * @param _serviceName Service name to send request
   * @param _callback Pointer to external method
   * @param _value0 Argument #0 passed into external method
   * @param _value1 Argument #1 passed into external method
   * @param _value2 Argument #2 passed into external method
   * @param _value3 Argument #3 passed into external method
   */
  template<typename _Interface,
           typename _Arg0,
           typename _Arg1,
           typename _Arg2,
           typename _Arg3,
           typename _Value0,
           typename _Value1,
           typename _Value2,
           typename _Value3>
  void call(const std::string &_serviceName,
            void(_Interface::*_callback)(_Arg0, _Arg1, _Arg2, _Arg3),
            _Value0 && _value0, _Value1 && _value1, _Value2 && _value2, _Value3 && _value3) {
    static_assert(std::is_abstract<_Interface>::value,
                  "_Interface is not an abstract class");
    push([=]() mutable {
      auto service = this->getService<_Interface>(_serviceName);
      if (service) {
        service->invoke([=]() mutable {
          (service.get()->*_callback)(_value0, _value1, _value2, _value3);
        });
      }
    });
  };

  /**
   * This method is used to call method from IService<>
   * @tparam _Interface Interface for calling method
   * @tparam _Arg0 Argument #0 type those is passed to external method
   * @tparam _Arg1 Argument #1 type those is passed to external method
   * @tparam _Arg2 Argument #2 type those is passed to external method
   * @tparam _Arg3 Argument #3 type those is passed to external method
   * @tparam _Arg4 Argument #4 type those is passed to external method
   * @param _serviceName Service name to send request
   * @param _callback Pointer to external method
   * @param _value0 Argument #0 passed into external method
   * @param _value1 Argument #1 passed into external method
   * @param _value2 Argument #2 passed into external method
   * @param _value3 Argument #3 passed into external method
   * @param _value4 Argument #4 passed into external method
   */
  template<typename _Interface,
           typename _Arg0,
           typename _Arg1,
           typename _Arg2,
           typename _Arg3,
           typename _Arg4,
           typename _Value0,
           typename _Value1,
           typename _Value2,
           typename _Value3,
           typename _Value4>
  void call(const std::string &_serviceName,
            void(_Interface::*_callback)(_Arg0, _Arg1, _Arg2, _Arg3, _Arg4),
            _Value0 && _value0, _Value1 && _value1, _Value2 && _value2, _Value3 && _value3, _Value4 && _value4) {
    static_assert(std::is_abstract<_Interface>::value,
                  "_Interface is not an abstract class");
    push([=]() mutable {
      auto service = this->getService<_Interface>(_serviceName);
      if (service) {
        service->invoke([=]() mutable {
          (service.get()->*_callback)(_value0, _value1, _value2, _value3, _value4);
        });
      }
    });
  };

  /**
   * This method is used to call method from IService<>
   * @tparam _Interface Interface for calling method
   * @tparam _Arg0 Argument #0 type those is passed to external method
   * @tparam _Arg1 Argument #1 type those is passed to external method
   * @tparam _Arg2 Argument #2 type those is passed to external method
   * @tparam _Arg3 Argument #3 type those is passed to external method
   * @tparam _Arg4 Argument #4 type those is passed to external method
   * @tparam _Arg5 Argument #5 type those is passed to external method
   * @param _serviceName Service name to send request
   * @param _callback Pointer to external method
   * @param _value0 Argument #0 passed into external method
   * @param _value1 Argument #1 passed into external method
   * @param _value2 Argument #2 passed into external method
   * @param _value3 Argument #3 passed into external method
   * @param _value4 Argument #4 passed into external method
   * @param _value5 Argument #5 passed into external method
   */
  template<typename _Interface,
           typename _Arg0,
           typename _Arg1,
           typename _Arg2,
           typename _Arg3,
           typename _Arg4,
           typename _Arg5,
           typename _Value0,
           typename _Value1,
           typename _Value2,
           typename _Value3,
           typename _Value4,
           typename _Value5>
  void call(const std::string &_serviceName,
            void(_Interface::*_callback)(_Arg0, _Arg1, _Arg2, _Arg3, _Arg4, _Arg5),
            _Value0 && _value0, _Value1 && _value1, _Value2 && _value2, _Value3 && _value3, _Value4 && _value4, _Value5 && _value5) {
    static_assert(std::is_abstract<_Interface>::value,
                  "_Interface is not an abstract class");
    push([=]() mutable {
      auto service = this->getService<_Interface>(_serviceName);
      if (service) {
        service->invoke([=]() mutable {
          (service.get()->*_callback)(_value0, _value1, _value2, _value3, _value4, _value5);
        });
      }
    });
  };

  /**
   * This method is used to subscribe on event from IService<>
   * @tparam _Interface Interface for subscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription without parameters
   */
  template<typename _Interface,
           typename _Client,
           typename _R>
  void subscribe(std::shared_ptr<_Client> _client,
                 const std::string & _serviceName,
                 Event<_R(void)> _Interface::*_event,
                 _R(_Client::*_callback)(void)) {
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
   * @tparam _Interface Interface for subscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Arg0 Argument #0 type those will appear in _callback method
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename _Arg0>
  void subscribe(std::shared_ptr<_Client> _client,
                 const std::string & _serviceName,
                 Event<_R(_Arg0)> _Interface::*_event,
                 _R(_Client::*_callback)(_Arg0)) {
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
   * @tparam _Interface Interface for subscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Arg0 Argument #0 type those will appear in _callback method
   * @tparam _Arg1 Argument #1 type those will appear in _callback method
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename _Arg0,
           typename _Arg1>
  void subscribe(std::shared_ptr<_Client> _client,
                 const std::string & _serviceName,
                 Event<_R(_Arg0, _Arg1)> _Interface::*_event,
                 _R(_Client::*_callback)(_Arg0, _Arg1)) {
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
   * @tparam _Interface Interface for subscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Arg0 Argument #0 type those will appear in _callback method
   * @tparam _Arg1 Argument #1 type those will appear in _callback method
   * @tparam _Arg2 Argument #2 type those will appear in _callback method
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename _Arg0,
           typename _Arg1,
           typename _Arg2>
  void subscribe(std::shared_ptr<_Client> _client,
                 const std::string & _serviceName,
                 Event<_R(_Arg0, _Arg1, _Arg2)> _Interface::*_event,
                 _R(_Client::*_callback)(_Arg0, _Arg1, _Arg2)) {
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
   * @tparam _Interface Interface for subscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Arg0 Argument #0 type those will appear in _callback method
   * @tparam _Arg1 Argument #1 type those will appear in _callback method
   * @tparam _Arg2 Argument #2 type those will appear in _callback method
   * @tparam _Arg3 Argument #3 type those will appear in _callback method
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename _Arg0,
           typename _Arg1,
           typename _Arg2,
           typename _Arg3>
  void subscribe(std::shared_ptr<_Client> _client,
                 const std::string & _serviceName,
                 Event<_R(_Arg0, _Arg1, _Arg2, _Arg3)> _Interface::*_event,
                 _R(_Client::*_callback)(_Arg0, _Arg1, _Arg2, _Arg3)) {
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
   * @tparam _Interface Interface for subscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Arg0 Argument #0 type those will appear in _callback method
   * @tparam _Arg1 Argument #1 type those will appear in _callback method
   * @tparam _Arg2 Argument #2 type those will appear in _callback method
   * @tparam _Arg3 Argument #3 type those will appear in _callback method
   * @tparam _Arg4 Argument #4 type those will appear in _callback method
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename _Arg0,
           typename _Arg1,
           typename _Arg2,
           typename _Arg3,
           typename _Arg4>
  void subscribe(std::shared_ptr<_Client> _client,
                 const std::string & _serviceName,
                 Event<_R(_Arg0, _Arg1, _Arg2, _Arg3, _Arg4)> _Interface::*_event,
                 _R(_Client::*_callback)(_Arg0, _Arg1, _Arg2, _Arg3, _Arg4)) {
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
   * @tparam _Interface Interface for subscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription without parameters
   */
  template<typename _Interface,
           typename _Client,
           typename _R>
  void subscribe(std::shared_ptr<IClient<_Interface>> _client,
                 const std::string & _serviceName,
                 Event<_R(void)> _Interface::*_event,
                 _R(_Client::*_callback)(void)) {
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
   * This method is used to subscribe on event from IService<>
   * @tparam _Interface Interface for subscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Arg0 Argument #0 type those will appear in _callback method
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename _Arg0>
  void subscribe(std::shared_ptr<IClient<_Interface>> _client,
                 const std::string & _serviceName,
                 Event<_R(_Arg0)> _Interface::*_event,
                 _R(_Client::*_callback)(_Arg0)) {
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
   * This method is used to subscribe on event from IService<>
   * @tparam _Interface Interface for subscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Arg0 Argument #0 type those will appear in _callback method
   * @tparam _Arg1 Argument #1 type those will appear in _callback method
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename _Arg0,
           typename _Arg1>
  void subscribe(std::shared_ptr<IClient<_Interface>> _client,
                 const std::string & _serviceName,
                 Event<_R(_Arg0, _Arg1)> _Interface::*_event,
                 _R(_Client::*_callback)(_Arg0, _Arg1)) {
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
   * This method is used to subscribe on event from IService<>
   * @tparam _Interface Interface for subscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Arg0 Argument #0 type those will appear in _callback method
   * @tparam _Arg1 Argument #1 type those will appear in _callback method
   * @tparam _Arg2 Argument #2 type those will appear in _callback method
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename _Arg0,
           typename _Arg1,
           typename _Arg2>
  void subscribe(std::shared_ptr<IClient<_Interface>> _client,
                 const std::string & _serviceName,
                 Event<_R(_Arg0, _Arg1, _Arg2)> _Interface::*_event,
                 _R(_Client::*_callback)(_Arg0, _Arg1, _Arg2)) {
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
   * This method is used to subscribe on event from IService<>
   * @tparam _Interface Interface for subscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Arg0 Argument #0 type those will appear in _callback method
   * @tparam _Arg1 Argument #1 type those will appear in _callback method
   * @tparam _Arg2 Argument #2 type those will appear in _callback method
   * @tparam _Arg3 Argument #3 type those will appear in _callback method
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename _Arg0,
           typename _Arg1,
           typename _Arg2,
           typename _Arg3>
  void subscribe(std::shared_ptr<IClient<_Interface>> _client,
                 const std::string & _serviceName,
                 Event<_R(_Arg0, _Arg1, _Arg2, _Arg3)> _Interface::*_event,
                 _R(_Client::*_callback)(_Arg0, _Arg1, _Arg2, _Arg3)) {
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
   * This method is used to subscribe on event from IService<>
   * @tparam _Interface Interface for subscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Arg0 Argument #0 type those will appear in _callback method
   * @tparam _Arg1 Argument #1 type those will appear in _callback method
   * @tparam _Arg2 Argument #2 type those will appear in _callback method
   * @tparam _Arg3 Argument #3 type those will appear in _callback method
   * @tparam _Arg4 Argument #4 type those will appear in _callback method
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename _Arg0,
           typename _Arg1,
           typename _Arg2,
           typename _Arg3,
           typename _Arg4>
  void subscribe(std::shared_ptr<IClient<_Interface>> _client,
                 const std::string & _serviceName,
                 Event<_R(_Arg0, _Arg1, _Arg2, _Arg3, _Arg4)> _Interface::*_event,
                 _R(_Client::*_callback)(_Arg0, _Arg1, _Arg2, _Arg3, _Arg4)) {
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
   * @tparam _Interface Interface for unsubscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription without parameters
   */
  template<typename _Interface,
           typename _Client,
           typename _R>
  void unsubscribe(std::shared_ptr<_Client> _client,
                   const std::string & _serviceName,
                   Event<_R(void)> _Interface::*_event,
                   _R(_Client::*_callback)(void)) {
    static_assert(std::is_base_of<IComponent, _Client>::value,
                  "IComponent is not a base class of _Client");
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
   * @tparam _Interface Interface for unsubscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Arg0 Argument #0 type those will appear in _callback method
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename _Arg0>
  void unsubscribe(std::shared_ptr<_Client> _client,
                   const std::string & _serviceName,
                   Event<_R(_Arg0)> _Interface::*_event,
                   _R(_Client::*_callback)(_Arg0)) {
    static_assert(std::is_base_of<IComponent, _Client>::value,
                  "IComponent is not a base class of _Client");
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
   * @tparam _Interface Interface for unsubscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Arg0 Argument #0 type those will appear in _callback method
   * @tparam _Arg1 Argument #1 type those will appear in _callback method
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename _Arg0,
           typename _Arg1>
  void unsubscribe(std::shared_ptr<_Client> _client,
                   const std::string & _serviceName,
                   Event<_R(_Arg0, _Arg1)> _Interface::*_event,
                   _R(_Client::*_callback)(_Arg0, _Arg1)) {
    static_assert(std::is_base_of<IComponent, _Client>::value,
                  "IComponent is not a base class of _Client");
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
   * @tparam _Interface Interface for unsubscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Arg0 Argument #0 type those will appear in _callback method
   * @tparam _Arg1 Argument #1 type those will appear in _callback method
   * @tparam _Arg2 Argument #2 type those will appear in _callback method
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename _Arg0,
           typename _Arg1,
           typename _Arg2>
  void unsubscribe(std::shared_ptr<_Client> _client,
                   const std::string & _serviceName,
                   Event<_R(_Arg0, _Arg1, _Arg2)> _Interface::*_event,
                   _R(_Client::*_callback)(_Arg0, _Arg1, _Arg2)) {
    static_assert(std::is_base_of<IComponent, _Client>::value,
                  "IComponent is not a base class of _Client");
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
   * @tparam _Interface Interface for unsubscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Arg0 Argument #0 type those will appear in _callback method
   * @tparam _Arg1 Argument #1 type those will appear in _callback method
   * @tparam _Arg2 Argument #2 type those will appear in _callback method
   * @tparam _Arg3 Argument #3 type those will appear in _callback method
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename _Arg0,
           typename _Arg1,
           typename _Arg2,
           typename _Arg3>
  void unsubscribe(std::shared_ptr<_Client> _client,
                   const std::string & _serviceName,
                   Event<_R(_Arg0, _Arg1, _Arg2, _Arg3)> _Interface::*_event,
                   _R(_Client::*_callback)(_Arg0, _Arg1, _Arg2, _Arg3)) {
    static_assert(std::is_base_of<IComponent, _Client>::value,
                  "IComponent is not a base class of _Client");
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
   * @tparam _Interface Interface for unsubscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Arg0 Argument #0 type those will appear in _callback method
   * @tparam _Arg1 Argument #1 type those will appear in _callback method
   * @tparam _Arg2 Argument #2 type those will appear in _callback method
   * @tparam _Arg3 Argument #3 type those will appear in _callback method
   * @tparam _Arg4 Argument #4 type those will appear in _callback method
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename _Arg0,
           typename _Arg1,
           typename _Arg2,
           typename _Arg3,
           typename _Arg4>
  void unsubscribe(std::shared_ptr<_Client> _client,
                   const std::string & _serviceName,
                   Event<_R(_Arg0, _Arg1, _Arg2, _Arg3, _Arg4)> _Interface::*_event,
                   _R(_Client::*_callback)(_Arg0, _Arg1, _Arg2, _Arg3, _Arg4)) {
    static_assert(std::is_base_of<IComponent, _Client>::value,
                  "IComponent is not a base class of _Client");
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
   * @tparam _Interface Interface for unsubscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription without parameters
   */
  template<typename _Interface,
           typename _Client,
           typename _R>
  void unsubscribe(std::shared_ptr<IClient<_Interface>> _client,
                   const std::string & _serviceName,
                   Event<_R(void)> _Interface::*_event,
                   _R(_Client::*_callback)(void)) {
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
   * This method is used to unsubscribe on event from IService<>
   * @tparam _Interface Interface for unsubscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Arg0 Argument #0 type those will appear in _callback method
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename _Arg0>
  void unsubscribe(std::shared_ptr<IClient<_Interface>> _client,
                 const std::string & _serviceName,
                 Event<_R(_Arg0)> _Interface::*_event,
                 _R(_Client::*_callback)(_Arg0)) {
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
   * This method is used to unsubscribe on event from IService<>
   * @tparam _Interface Interface for unsubscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Arg0 Argument #0 type those will appear in _callback method
   * @tparam _Arg1 Argument #1 type those will appear in _callback method
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename _Arg0,
           typename _Arg1>
  void unsubscribe(std::shared_ptr<IClient<_Interface>> _client,
                 const std::string & _serviceName,
                 Event<_R(_Arg0, _Arg1)> _Interface::*_event,
                 _R(_Client::*_callback)(_Arg0, _Arg1)) {
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
   * This method is used to unsubscribe on event from IService<>
   * @tparam _Interface Interface for unsubscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Arg0 Argument #0 type those will appear in _callback method
   * @tparam _Arg1 Argument #1 type those will appear in _callback method
   * @tparam _Arg2 Argument #2 type those will appear in _callback method
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename _Arg0,
           typename _Arg1,
           typename _Arg2>
  void unsubscribe(std::shared_ptr<IClient<_Interface>> _client,
                 const std::string & _serviceName,
                 Event<_R(_Arg0, _Arg1, _Arg2)> _Interface::*_event,
                 _R(_Client::*_callback)(_Arg0, _Arg1, _Arg2)) {
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
   * This method is used to unsubscribe on event from IService<>
   * @tparam _Interface Interface for unsubscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Arg0 Argument #0 type those will appear in _callback method
   * @tparam _Arg1 Argument #1 type those will appear in _callback method
   * @tparam _Arg2 Argument #2 type those will appear in _callback method
   * @tparam _Arg3 Argument #3 type those will appear in _callback method
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename _Arg0,
           typename _Arg1,
           typename _Arg2,
           typename _Arg3>
  void unsubscribe(std::shared_ptr<IClient<_Interface>> _client,
                 const std::string & _serviceName,
                 Event<_R(_Arg0, _Arg1, _Arg2, _Arg3)> _Interface::*_event,
                 _R(_Client::*_callback)(_Arg0, _Arg1, _Arg2, _Arg3)) {
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
   * This method is used to unsubscribe on event from IService<>
   * @tparam _Interface Interface for unsubscribing on _event
   * @tparam _Client Client type in which located callback
   * @tparam _R Return value of Event<>
   * @tparam _Arg0 Argument #0 type those will appear in _callback method
   * @tparam _Arg1 Argument #1 type those will appear in _callback method
   * @tparam _Arg2 Argument #2 type those will appear in _callback method
   * @tparam _Arg3 Argument #3 type those will appear in _callback method
   * @tparam _Arg4 Argument #4 type those will appear in _callback method
   * @param _client Client object that tries to subscribe on _event
   * @param _serviceName Service name to send request
   * @param _event Event for subscription
   * @param _callback Callback for subscription
   */
  template<typename _Interface,
           typename _Client,
           typename _R,
           typename _Arg0,
           typename _Arg1,
           typename _Arg2,
           typename _Arg3,
           typename _Arg4>
  void unsubscribe(std::shared_ptr<IClient<_Interface>> _client,
                   const std::string & _serviceName,
                   Event<_R(_Arg0, _Arg1, _Arg2, _Arg3, _Arg4)> _Interface::*_event,
                   _R(_Client::*_callback)(_Arg0, _Arg1, _Arg2, _Arg3, _Arg4)) {
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

 protected:
  /**
   * Helper function for finding service in ProcessBus tables
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
  ProcessBus();
  ProcessBus(const ProcessBus &) = delete;
  ProcessBus & operator=(const ProcessBus &) = delete;
  ProcessBus(ProcessBus &&) = delete;
  ProcessBus & operator=(ProcessBus &&) = delete;
  virtual ~ProcessBus();

 private:
  std::thread thread_;
  std::map<tKeyForClientList, tListOfClients> clients_;
  std::map<tKeyForServiceList, tListOfServices> services_;
};

}

}

#endif //ICC_PROCESSBUS_HPP
