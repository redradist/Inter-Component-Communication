/**
 * @file CommonAPIClient.hpp
 * @author Denis Kotov
 * @date 3 Apr 2017
 * @brief Contains CommonAPIClient wrapper class for creating client
 * @copyright MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_COMMONAPI_CLIENT_COMPONENT_HPP
#define ICC_COMMONAPI_CLIENT_COMPONENT_HPP

#include <CommonAPI/CommonAPI.hpp>
#include <type_traits>
#include <helpers/memory_helpers.hpp>
#include <logger/DummyLogger.hpp>

namespace icc {

namespace commonapi {

template< template< typename ... _AttributeExtensions > class Proxy,
          typename Logger = icc::logger::DummyLogger >
class CommonAPIClient
    : public icc::helpers::virtual_enable_shared_from_this< CommonAPIClient<Proxy, Logger> >
    , public virtual Logger {
  static_assert(std::is_base_of<CommonAPI::Proxy, Proxy<>>::value,
                "Proxy does not derived from CommonAPI::Proxy");
 public:
  CommonAPIClient() {
    Logger::debug("Constructor CommonAPIClient()");
  }

  CommonAPIClient(const std::string &_domain,
                  const std::string &_instance)
    : domain_(_domain)
    , instance_(_instance) {
    Logger::debug("Constructor CommonAPIClient(const std::string &_domain, const std::string &_instance)");
  }

  CommonAPIClient(CommonAPIClient const &) = default;
  CommonAPIClient & operator=(CommonAPIClient const &) = default;
  CommonAPIClient(CommonAPIClient &&) = delete;
  CommonAPIClient & operator=(CommonAPIClient &&) = delete;

  virtual ~CommonAPIClient() {
    Logger::debug("Destructor CommonAPIClient");
  }

 public:
  /**
   *
   */
  void initClient(const std::string & _domain = std::string(),
                  const std::string & _instance = std::string()) {
    if (not proxy_ptr_ && !domain_.empty() && !instance_.empty()) {
      proxy_ptr_ = std::unique_ptr< Proxy<> >(new Proxy<>([=]() {
        Logger::debug("Building CommonAPIClient ...");
        std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
        auto proxy = runtime->buildProxy<Proxy>(domain_, instance_);
        if (!proxy) {
          Logger::error("proxy is nullptr");
        } else {
          Logger::debug("CommonAPIClient is built successfully !!");
          std::weak_ptr<CommonAPIClient> weakClient = this->shared_from_this();
          proxy->getProxyStatusEvent().subscribe(
          [=](const CommonAPI::AvailabilityStatus & _status) mutable {
            if (auto client = weakClient.lock()) {
              if (CommonAPI::AvailabilityStatus::AVAILABLE == _status) {
                Logger::debug("CommonAPIClient is connected");
                connected(*proxy);
              } else {
                Logger::debug("CommonAPIClient is disconnected");
                disconnected(*proxy);
              }
            }
          });
        }
        return proxy;
      }()));
    }
  }

  /**
   *
   */
  void reinitClient() {
    proxy_ptr_.reset();
    initClient();
  }

  /**
   *
   */
  void deinitClient() {
    proxy_ptr_.reset();
  }

  virtual void connected(Proxy<> &) = 0;
  virtual void disconnected(Proxy<> &) = 0;

 protected:
  std::unique_ptr< Proxy<> > proxy_ptr_;

 private:
  std::string domain_;
  std::string instance_;
};

}

}

#endif // ICC_COMMONAPI_CLIENT_COMPONENT_HPP
