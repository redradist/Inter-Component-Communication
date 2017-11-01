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

namespace icc {

namespace commonapi {

template<template<typename ... _AttributeExtensions> class Proxy>
class CommonAPIClient
    : public Proxy<> {
  static_assert(std::is_base_of<CommonAPI::Proxy, Proxy<>>::value,
                "Proxy does not derived from CommonAPI::Proxy");
 public:
  CommonAPIClient(const std::string &_domain,
                  const std::string &_instance) :
      Proxy<>([=]() {
        std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
        auto proxy = runtime->buildProxy<Proxy>(_domain, _instance);
        return proxy;
      }()) {
  }

  CommonAPIClient(CommonAPIClient const &) = default;
  CommonAPIClient(CommonAPIClient &&) = default;
};

}

}

#endif // ICC_COMMONAPI_CLIENT_COMPONENT_HPP
