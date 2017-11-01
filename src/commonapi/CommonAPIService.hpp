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

namespace icc {

namespace commonapi {

template<typename Service>
class CommonAPIService
    : public Service {
  static_assert(icc::helpers::is_base_of_template<Service, CommonAPI::Stub>::value,
                "Service does not derived from CommonAPI::Stub");
 public:
  CommonAPIService(const std::string &_domain,
                   const std::string &_instance)
      : domain_(_domain),
        instance_(_instance) {
    registerService(_domain, _instance);
  }

  CommonAPIService(CommonAPIService const &_service) = delete;
  CommonAPIService(const Service &_service) = delete;

  ~CommonAPIService() {
    unregisterService();
  }

 public:
  bool registerService(const std::string &_domain,
                       const std::string &_instance) {
    bool isRegistered = false;
    if (!service_ && (_domain != domain_ || _instance != instance_)) {
      std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
      service_ = std::shared_ptr<Service>(this, [](Service *) {
        // Do nothing to prevent double deletion
      });
      isRegistered = runtime->registerService(_domain, _instance, service_);
    }
    return isRegistered;
  }

  bool unregisterService() {
    bool isUnregistered = false;
    std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
    if (isUnregistered = runtime->unregisterService(domain_, this->getStubAdapter()->getInterface(), instance_)) {
      service_ = nullptr;
    }
    return isUnregistered;
  }

 private:
  const std::string domain_;
  const std::string instance_;
  std::shared_ptr<Service> service_ = nullptr;
};

}

}

#endif  // ICC_COMMONAPISERVICE_HPP
