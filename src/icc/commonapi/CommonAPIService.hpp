/**
 * @file CommonAPIService.hpp
 * @author Denis Kotov
 * @date 3 Apr 2017
 * @brief Contains CommonAPIService wrapper class for creating service
 * @copyright MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_COMMONAPISERVICE_HPP
#define ICC_COMMONAPISERVICE_HPP

#include <CommonAPI/CommonAPI.hpp>
#include <icc/_private/helpers/concept_base_of_template.hpp>
#include <icc/_private/helpers/memory_helpers.hpp>
#include <icc/Component.hpp>
#include <icc/logger/DummyLogger.hpp>

namespace icc {

namespace commonapi {

template< typename Service,
          typename Logger = icc::logger::DummyLogger >
class CommonAPIService
    : public virtual Component
    , public Service
    , public virtual Logger
    , public icc::helpers::virtual_enable_shared_from_this< CommonAPIService<Service, Logger> >{
  static_assert(icc::helpers::is_base_of_template<Service, CommonAPI::Stub>::value,
                "Service does not derived from CommonAPI::Stub");
 protected:
  CommonAPIService()
    : Component(nullptr) {
    Logger::debug("Constructor CommonAPIService()");
  }

  CommonAPIService(const std::string &_domain,
                   const std::string &_instance)
    : Component(nullptr)
    , domain_(_domain)
    , instance_(_instance) {
    Logger::debug("Constructor CommonAPIService(const std::string &_domain, const std::string &_instance)");
    registerService(domain_, instance_);
  }

  CommonAPIService(CommonAPIService const &_service) = delete;
  CommonAPIService(const Service &_service) = delete;

  virtual ~CommonAPIService() {
    Logger::debug("Destructor CommonAPIService");
    unregisterService();
  }

 public:
  /**
   * Method for registering the service
   * @param _domain Domain name
   * @param _instance Instance string
   */
  void registerService(const std::string &_domain,
                       const std::string &_instance) {
    invoke([=] () mutable {
      if (is_registered_) {
        Logger::warning("CommonAPIService is already registered !!!");
        Logger::warning("Unregister CommonAPIService first !?");
      } else {
        Logger::debug("Registering CommonAPIService ...");
        std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
        is_registered_ = runtime->registerService(_domain, _instance, this->shared_from_this());
        if (is_registered_) {
          Logger::debug("CommonAPIService registered successfully !!");
          domain_ = _domain;
          instance_ = _instance;
        } else {
          Logger::error("Failed to register CommonAPIService !!");
        }
      }
    });
  }

  /**
   * Method for unregistering the service
   */
  void unregisterService() {
    invoke([=] () mutable {
      if (!is_registered_) {
        Logger::warning("CommonAPIService is not registered !!!");
        Logger::warning("Register CommonAPIService before unregistering !?");
      } else {
        Logger::debug("Unregistering CommonAPIService ...");
        std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
        is_registered_ = !runtime->unregisterService(domain_, this->getStubAdapter()->getInterface(), instance_);
        if (!is_registered_) {
          Logger::debug("CommonAPIService unregistered successfully !!");
        } else {
          Logger::error("Failed to unregister CommonAPIService !!");
        }
      }
    });
  }

 private:
  bool is_registered_ = false;
  std::string domain_;
  std::string instance_;
};

}

}

#endif  // ICC_COMMONAPISERVICE_HPP
