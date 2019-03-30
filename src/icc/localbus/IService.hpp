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
#include <icc/IComponent.hpp>
#include <icc/helpers/memory_helpers.hpp>
#include "ProcessBus.hpp"

namespace icc {

namespace localbus {

template <typename _Interface>
class IService
  : public virtual IComponent,
    public _Interface,
    public icc::helpers::virtual_enable_shared_from_this<IService<_Interface>> {
  static_assert(std::is_abstract<_Interface>::value,
                "_Interface is not an abstract class");
 public:
  /**
   * Default constructor
   */
  IService()
    : IComponent(nullptr) {
  }
  /**
   * Constructor which register the service
   * @param _serviceName Service name, should be unique in the process
   */
  IService(const std::string & _serviceName)
    : IComponent(nullptr) {
    registerService(_serviceName);
  }

  /**
   * Destructor which unregister the service
   */
  virtual ~IService() = 0;

 public:
  /**
   * Used to register IService<_Interface>
   */
  void registerService(const std::string & _serviceName) {
    invoke([=] {
      if (service_name_.empty() && !_serviceName.empty()) {
        service_name_ = _serviceName;
        ProcessBus::getBus().registerService(this->shared_from_this(), service_name_);
      }
    });
  }

  /**
   * Used to unregister IService<_Interface>
   */
  void unregisterService() {
    invoke([=] {
      if (!service_name_.empty()) {
        ProcessBus::getBus().unregisterService(this->shared_from_this(), service_name_);
        service_name_.clear();
      }
    });
  }

 private:
  std::shared_ptr< IService > holder_ = std::shared_ptr< IService >(this, [](IService*) {
                                                                            // NOTE(redra): Nothing need to do. Just create holder for responses and broadcasts
                                                                          });
  std::string service_name_;
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
