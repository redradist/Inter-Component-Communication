/**
 * @file Event.hpp
 * @author Denis Kotov
 * @date 25 Jun 2017
 * @brief Contains IClient interface.
 * It is thread safe version of class like boost::signal and boost::signal2.
 * Safety guarantee on client side
 * @copyright Denis Kotov, MIT License. Open source:
 */

#ifndef ICC_ISERVICECLIENT_HPP
#define ICC_ISERVICECLIENT_HPP

#include <type_traits>
#include "../IComponent.hpp"
#include "ProcessBus.hpp"

template <typename _Interface>
class IClient
  : virtual public IComponent {
  static_assert(std::is_abstract<_Interface>::value,
                "_Interface is not an abstract class");
 public:
  IClient(const std::string & _serviceName)
    : service_name_(_serviceName) {
    ProcessBus::getBus().buildClient(this, _serviceName);
  }

 public:
  virtual void connected(_Interface&) = 0;
  virtual void disconnected(_Interface&) = 0;

  template <typename ... _Args>
  void call(void(_Interface::*_callback)(_Args...), _Args ... _args) {
    push([=]{
      (service_->*_callback)(std::forward<_Args>(_args)...);
    });
  };

 protected:
  void setService(_Interface * _service) {
    service_ = _service;
  }

 private:
  friend class ProcessBus;
  const std::string service_name_;
  _Interface * service_;
};

#endif //ICC_ISERVICECLIENT_HPP
