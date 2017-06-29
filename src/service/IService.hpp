/**
 * @file Event.hpp
 * @author Denis Kotov
 * @date 25 Jun 2017
 * @brief Contains IService interface.
 * It is used to provide service which is implemented with
 * _Interface interface.
 * @copyright Denis Kotov, MIT License. Open source:
 */

#ifndef ICC_SERVICE_HPP
#define ICC_SERVICE_HPP

#include <type_traits>
#include <IComponent.hpp>
#include "ProcessBus.hpp"

template <typename _Interface>
class IService
  : public IComponent,
    public _Interface {
  static_assert(std::is_abstract<_Interface>::value,
                "_Interface is not an abstract class");
 public:
  IService(const std::string & _serviceName)
      : service_name_(_serviceName) {
    ProcessBus::getBus().registerService(this, service_name_);
  }

  ~IService() {
    //ProcessBus::getBus().unregisterService(this, service_name_);
  }

 private:
  const std::string service_name_;
};

#endif //ICC_SERVICE_HPP
