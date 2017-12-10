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
#include <helpers/concept_base_of_template.hpp>
#include <logger/DummyLogger.hpp>

namespace icc {

namespace commonapi {

template< typename Service,
          typename Logger = icc::logger::DummyLogger >
class CommonAPIService
    : public Service
    , public virtual Logger {
  static_assert(icc::helpers::is_base_of_template<Service, CommonAPI::Stub>::value,
                "Service does not derived from CommonAPI::Stub");
 public:
  CommonAPIService() = default;
  CommonAPIService(const std::string &_domain,
                   const std::string &_instance)
      : domain_(_domain),
        instance_(_instance) {
    Logger::debug("Constructor CommonAPIService");
    registerService(domain_, instance_);
  }

  CommonAPIService(CommonAPIService const &_service) = delete;
  CommonAPIService(const Service &_service) = delete;

  virtual ~CommonAPIService() {
    Logger::debug("Destructor CommonAPIService");
    unregisterService();
  }

 public:
  bool registerService(const std::string &_domain,
                       const std::string &_instance) {
    if (is_registered) {
      Logger::warning("CommonAPIService is already registered !!!");
      Logger::warning("Unregister CommonAPIService first !?");
    } else {
      Logger::debug("Registering CommonAPIService ...");
      std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
      service_ = std::shared_ptr<Service>(this, [](Service *) {
        // Do nothing to prevent double deletion
      });
      if (service_) {
        is_registered = runtime->registerService(_domain, _instance, service_);
        if (is_registered) {
          Logger::debug("CommonAPIService registered successfully !!");
          domain_ = _domain;
          instance_ = _instance;
        } else {
          Logger::error("Failed to register CommonAPIService !!");
          service_ = nullptr;
        }
      }
    }
    return is_registered;
  }

  bool unregisterService() {
    if (!is_registered) {
      Logger::warning("CommonAPIService is not registered !!!");
      Logger::warning("Register CommonAPIService before unregistering !?");
    } else {
      Logger::debug("Unregistering CommonAPIService ...");
      std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
      is_registered = !runtime->unregisterService(domain_, this->getStubAdapter()->getInterface(), instance_);
      if (!is_registered) {
        Logger::debug("CommonAPIService unregistered successfully !!");
        service_ = nullptr;
      } else {
        Logger::error("Failed to unregister CommonAPIService !!");
      }
    }
    return !is_registered;
  }

 private:
  bool is_registered = false;
  std::string domain_;
  std::string instance_;
  std::shared_ptr<Service> service_ = nullptr;
};

}

}

#endif  // ICC_COMMONAPISERVICE_HPP
