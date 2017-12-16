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
#include <logger/DummyLogger.hpp>

namespace icc {

namespace commonapi {

template< template< typename ... _AttributeExtensions > class Proxy,
          typename Logger = icc::logger::DummyLogger >
class CommonAPIClient
    : public Proxy<>
    , public virtual Logger {
  static_assert(std::is_base_of<CommonAPI::Proxy, Proxy<>>::value,
                "Proxy does not derived from CommonAPI::Proxy");
 public:
  CommonAPIClient(const std::string &_domain,
                  const std::string &_instance) :
      Proxy<>([=]() {
        this->debug("Building CommonAPIClient ...");
        std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
        auto proxy = runtime->buildProxy<Proxy>(_domain, _instance);
        if (!proxy) {
          this->error("proxy is nullptr");
        } else {
          this->debug("CommonAPIClient is built successfully !!");
          proxy->getProxyStatusEvent().subscribe(
          [=](const CommonAPI::AvailabilityStatus & _status) mutable {
            if (CommonAPI::AvailabilityStatus::AVAILABLE == _status) {
              this->debug("CommonAPIClient is connected");
              connected(*this);
            } else {
              this->debug("CommonAPIClient is disconnected");
              disconnected(*this);
            }
          });
        }
        return proxy;
      }()) {
    Logger::debug("Constructor CommonAPIClient");
  }

  CommonAPIClient(CommonAPIClient const &) = default;
  CommonAPIClient & operator=(CommonAPIClient const &) = default;
  CommonAPIClient(CommonAPIClient &&) = delete;
  CommonAPIClient & operator=(CommonAPIClient &&) = delete;

  virtual ~CommonAPIClient() = default;

  virtual void connected(Proxy<> &) = 0;
  virtual void disconnected(Proxy<> &) = 0;
};

}

}

#endif // ICC_COMMONAPI_CLIENT_COMPONENT_HPP
