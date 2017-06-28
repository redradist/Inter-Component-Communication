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

template <typename _Interface>
class IService;

template <typename _Interface>
class IClient
  : virtual public IComponent {
  static_assert(std::is_abstract<_Interface>::value,
                "_Interface is not an abstract class");
 public:
  IClient(const std::string & _serviceName)
    : service_name_(_serviceName) {
    ProcessBus::getBus().buildClient(this, service_name_);
  }

  ~IClient() {
    ProcessBus::getBus().disassembleClient(this, service_name_);
  }

 public:
  virtual void connected(_Interface&) = 0;
  virtual void disconnected(_Interface&) = 0;

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

 protected:
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
