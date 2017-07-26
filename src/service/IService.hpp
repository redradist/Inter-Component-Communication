/**
 * @file IService.hpp
 * @author Denis Kotov
 * @date 25 Jun 2017
 * @brief Contains IService interface.
 * It is used to provide service which is implemented with
 * _Interface interface.
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_SERVICE_HPP
#define ICC_SERVICE_HPP

#include <type_traits>
#include <IComponent.hpp>
#include <helpers/memory_helpers.hpp>
#include "ProcessBus.hpp"

namespace icc {

namespace service {

template <typename _Interface>
class IService
  : public virtual IComponent,
    public _Interface,
    public icc::helpers::virtual_enable_shared_from_this<IService<_Interface>> {
  static_assert(std::is_abstract<_Interface>::value,
                "_Interface is not an abstract class");
 public:
  /**
   * Constructor which register the service
   * @param _serviceName Service name, should be unique in the process
   */
  IService(const std::string & _serviceName)
      : service_name_(_serviceName) {
  }

  /**
   * Destructor which unregister the service
   */
  virtual ~IService() = 0;

 public:
  /**
   * Used to register IService<_Interface>
   */
  void registerService() {
    ProcessBus::getBus().registerService(this->shared_from_this(), service_name_);
  }

  /**
   * Used to unregister IService<_Interface>
   */
  void unregisterService() {
    ProcessBus::getBus().unregisterService(this->shared_from_this(), service_name_);
  }

 private:
  const std::string service_name_;
};

/**
 * Destructor of the service
 */
template <typename _Interface>
inline
IService<_Interface>::~IService() {
}

}

}

#endif //ICC_SERVICE_HPP
