/**
 * @file Event.hpp
 * @author Denis Kotov
 * @date 25 Jun 2017
 * @brief Contains IClient interface.
 * It is used to connect service which is implemented by
 * _Interface interface.
 * @copyright Denis Kotov, MIT License. Open source:
 */

#ifndef ICC_ISERVICECLIENT_HPP
#define ICC_ISERVICECLIENT_HPP

#include <type_traits>
#include <IComponent.hpp>
#include "ProcessBus.hpp"

template <typename _T>
class Event;

template <typename _Interface>
class IService;

template <typename _Interface>
class IClient
  : public virtual IComponent {
  static_assert(std::is_abstract<_Interface>::value,
                "_Interface is not an abstract class");
 public:
  /**
   * Constructor which build the client
   * @param _serviceName Service name, should be unique in the process
   */
  IClient(const std::string & _serviceName)
    : service_name_(_serviceName) {
    ProcessBus::getBus().buildClient(this, service_name_);
  }

  /**
   * Constructor which build the client with external event loop
   * @param _serviceName Service name, should be unique in the process
   * @param _parent Parent object that provides event loop
   */
  IClient(const std::string & _serviceName, IComponent * _parent)
      : IComponent(_parent)
      , service_name_(_serviceName) {
    ProcessBus::getBus().buildClient(this, service_name_);
  }

  /**
   * Destructor which disassemble the client
   */
  ~IClient() {
    ProcessBus::getBus().disassembleClient(this, service_name_);
  }

 public:
  /**
   * This method will be called on connect client to service
   */
  virtual void connected(_Interface&) = 0;
  /**
   * This method will be called on disconnection client from service
   */
  virtual void disconnected(_Interface&) = 0;

  /**
   * This method is used to call method from IService<>
   * @tparam _Args Arguments types those is passed to external method
   * @param _callback Pointer to external method
   * @param _args Arguments passed into external method
   */
  template <typename ... _Args>
  void call(void(_Interface::*_callback)(_Args...), _Args ... _args) {
    push([=]{
      if (service_) {
        service_->push([=]{
          (service_->*_callback)(std::forward<_Args>(_args)...);
        });
      }
    });
  };

  /**
   * This method is used to call method from IService<>
   * @tparam _Params Arguments types for reply
   * @tparam _Args Arguments types for external method
   * @param _callback Pointer to external method
   * @param _reply Reply with some result from IService<>
   * @param _args Arguments passed into external method
   */
  template <typename ... _Params, typename ... _Args>
  void call(void(_Interface::*_callback)(std::function<void(_Params ...)>, _Args...),
            std::function<void(_Params ...)> _reply,
            _Args ... _args) {
    push([=]{
      if (service_) {
        std::function<void(_Params ...)> _safeReply =
        [=](_Params ... params){
          push([=]{
            _reply(params...);
          });
        };
        service_->push([=]{
          (service_->*_callback)(_safeReply, std::forward<_Args>(_args)...);
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
  template < typename _Client,
             typename _R,
             typename ... _Args >
  void subscribe(Event<_R(_Args...)> _Interface::*_event,
                 _R(_Client::*_callback)(_Args...)) {
    static_assert(std::is_base_of<IClient<_Interface>, _Client>::value,
                  "_Interface is not an abstract class");
    push([=]{
      if (service_) {
        service_->push([=]{
          (service_->*_event).connect(
              _callback,
              static_cast<_Client*>(this));
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
  template < typename _Client,
             typename _R,
             typename ... _Args >
  void unsubscribe(Event<_R(_Args...)> _Interface::*_event,
                   _R(_Client::*_callback)(_Args...)) {
    push([=]{
      if (service_) {
        service_->push([=]{
          (service_->*_event).disconnect(
              _callback,
              static_cast<_Client*>(this));
        });
      }
    });
  };

 protected:
  /**
   * Used by ProcessBus to set IService<>
   * @param _service IService<> for set
   */
  virtual void setService(IService<_Interface> * _service) {
    push([=]{
      service_ = _service;
      if (service_) {
        connected(*service_);
      } else {
        disconnected(*service_);
      }
    });
  }

 private:
  friend class ProcessBus;
  const std::string service_name_;
  IService<_Interface> * service_ = nullptr;
};

#endif //ICC_ISERVICECLIENT_HPP
