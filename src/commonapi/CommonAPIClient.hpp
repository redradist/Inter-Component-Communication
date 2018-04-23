/**
 * @file CommonAPIClient.hpp
 * @author Denis Kotov
 * @date 3 Apr 2017
 * @brief Contains CommonAPIClient wrapper class for creating client
 * @copyright MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_COMMONAPI_CLIENT_COMPONENT_HPP
#define ICC_COMMONAPI_CLIENT_COMPONENT_HPP

#include <memory>
#include <type_traits>
#include <boost/optional.hpp>
#include <CommonAPI/CommonAPI.hpp>
#include <IComponent.hpp>
#include <commonapi/exceptions/CommonAPIClientError.hpp>
#include <helpers/memory_helpers.hpp>
#include <logger/DummyLogger.hpp>

namespace icc {

namespace commonapi {

template< template< typename ... _AttributeExtensions > class Proxy,
          typename Logger = icc::logger::DummyLogger >
class CommonAPIClient
    : public virtual IComponent
    , public Proxy<>
    , public virtual Logger
    , public icc::helpers::virtual_enable_shared_from_this< CommonAPIClient<Proxy, Logger> > {
  static_assert(std::is_base_of<CommonAPI::Proxy, Proxy<>>::value,
                "Proxy does not derived from CommonAPI::Proxy");
 public:
  CommonAPIClient()
    : IComponent(nullptr)
    , Proxy<>(nullptr) {
    Logger::debug("CommonAPIClient()");
    is_inited_ = false;
  }

  CommonAPIClient(const std::string &_domain,
                  const std::string &_instance)
    : IComponent(nullptr)
    , Proxy<>([=]() {
      std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
      auto proxy = runtime->buildProxy<Proxy>(_domain, _instance);
      if (!proxy) {
        throw icc::commonapi::CommonAPIClientError("Failed to create CommonAPIClient !!");
      }
      return proxy;
    }()) {
    Logger::debug("Constructor CommonAPIClient(const std::string &_domain, const std::string &_instance)");
    is_inited_ = true;
  }

  virtual ~CommonAPIClient() {
    Logger::debug("Destructor CommonAPIClient");
  }

 protected:
  CommonAPIClient(const std::shared_ptr<CommonAPIClient> _client)
    : IComponent(_client->getEventLoop())
    , Proxy<>([=]() {
      return _client;
    }()) {
    Logger::debug("Copy constructor CommonAPIClient()");
    is_inited_ = true;
  }

 private:
  CommonAPIClient & operator=(const std::shared_ptr<CommonAPIClient> _client) {
    Logger::debug("operator= of CommonAPIClient()");
    Proxy<>::operator=(*_client);
    is_inited_ = true;
  }

  CommonAPIClient(CommonAPIClient const &) = delete;
  CommonAPIClient & operator=(CommonAPIClient const &) = delete;
  CommonAPIClient(CommonAPIClient &&) = delete;
  CommonAPIClient & operator=(CommonAPIClient &&) = delete;

 public:
  void subscribeOnServiceStatus() {
    invoke([=] {
      Logger::debug("subscribeOnServiceStatus is called");
      if (on_service_status_) {
        Logger::warning("Service Status is already subscribed !!");
      } else {
        Logger::debug("Subscribing on Service Status ...");
        std::weak_ptr<CommonAPIClient> weakClient = this->shared_from_this();
        on_service_status_ = Proxy<>::getProxyStatusEvent()
            .subscribe(
            [=](const CommonAPI::AvailabilityStatus & _status) mutable {
              if (auto client = weakClient.lock()) {
                push([=] {
                  if (auto client = weakClient.lock()) {
                    if (CommonAPI::AvailabilityStatus::AVAILABLE == _status) {
                      connected(*this);
                    } else {
                      disconnected(*this);
                    }
                  }
                });
              }
            });
      }
    });
  }

  void unsubscribeFromServiceStatus() {
    invoke([=] {
      Logger::debug("unsubscribeFromServiceStatus is called");
      if (!on_service_status_) {
        Logger::warning("Service Status is not subscribed !!");
      } else {
        Logger::debug("Unsubscribing on Service Status ...");
        Proxy<>::getProxyStatusEvent().unsubscribe(on_service_status_.get());
        on_service_status_.reset();
      }
    });
  }

  /**
   * Method for initialization of client
   * @param _domain Domain name
   * @param _instance Instance string
   */
  void initClient(const std::string & _domain,
                  const std::string & _instance) {
    invoke([=] {
      if (!is_inited_) {
        Logger::warning("CommonAPIClient is already inited !!!");
        Logger::warning("Deinit CommonAPIClient first !?");
      } else {
        if (!_domain.empty() && !_instance.empty()) {
          *this = std::make_shared<CommonAPIClient<Proxy, Logger>>(_domain, _instance);
          Logger::debug("CommonAPIClient inited successfully !!");
        } else {
          Logger::error("Failed to init CommonAPIClient !!");
        }
      }
    });
  }

  /**
   * Method for reinitialization of client
   * @param _domain Domain name
   * @param _instance Instance string
   */
  void reinitClient(const std::string & _domain,
                    const std::string & _instance) {
    invoke([=] {
      deinitClient();
      initClient(_domain, _instance);
    });
  }

  /**
   * Method for deinitialization of client
   */
  void deinitClient() {
    invoke([=] {
      disconnected(*this);
      *this = nullptr;
      is_inited_ = false;
    });
  }

  virtual void connected(Proxy<> &) = 0;

  virtual void disconnected(Proxy<> &) = 0;

 private:
  bool is_inited_ = false;

  /************************************************************
   * Below variables for storing subscriptions on broadcasts
   ***********************************************************/
  boost::optional<CommonAPI::Event<>::Subscription> on_service_status_;
};

}

}

#endif // ICC_COMMONAPI_CLIENT_COMPONENT_HPP
