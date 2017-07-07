/**
 * @file Event.hpp
 * @author Denis Kotov
 * @date 25 Jun 2017
 * @brief Contains IClient interface.
 * It is used to connect service which is implemented by
 * _Interface interface.
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_ISERVICECLIENT_HPP
#define ICC_ISERVICECLIENT_HPP

#include <utility>
#include <type_traits>
#include <IComponent.hpp>
#include <helpers/memory_helpers.hpp>
#include "ProcessBus.hpp"

namespace icc {

namespace service {

template<typename _Interface>
class IClient
    : public virtual IComponent,
      public std::virtual_enable_shared_from_this<IClient<_Interface>> {
  static_assert(std::is_abstract<_Interface>::value,
                "_Interface is not an abstract class");
 public:
  /**
   * Constructor which build the client
   * @param _serviceName Service name, should be unique in the process
   */
  IClient(const std::string &_serviceName)
      : service_name_(_serviceName) {
    ProcessBus::getBus().buildClient(this, service_name_);
  }

  /**
   * Destructor which disassemble the client
   */
  virtual ~IClient() = 0;

 public:
  /**
   * This method will be called on connect client to service
   */
  virtual void connected(_Interface *) = 0;
  /**
   * This method will be called on disconnection client from service
   */
  virtual void disconnected(_Interface *) = 0;

  /**
   * This method is used to call method from IService<>
   * @tparam _Callback Type of callback functor
   * @tparam _Values Values types those is passed to external method
   * @param _callback Pointer to external method
   * @param _values Arguments passed into external method
   */
  template<typename _Callback, typename ... _Values>
  void call(_Callback && _callback, _Values && ... _values) {
    ProcessBus::getBus().call(service_name_,
                              std::forward<_Callback>(_callback),
                              std::forward<_Values>(_values)...);
  };

  /**
   * This method is used to call method from IService<>
   * @tparam _Callback Type of callback functor
   * @tparam _Reply Type of reply functor
   * @tparam _Values Values types those is passed to external method
   * @param _callback Pointer to external method
   * @param _reply Reply with some result from IService<>
   * @param _values Arguments passed into external method
   */
  template<typename _Callback, typename _Reply, typename ... _Values>
  void call(_Callback && _callback, _Reply && _reply, _Values && ... _values) {
    ProcessBus::getBus().call(service_name_,
                              std::forward<_Callback>(_callback),
                              std::forward<_Reply>(_reply),
                              std::forward<_Values>(_values)...);
  };

  /**
   * This method is used to subscribe on event from IService<>
   * @tparam _Event Event object to subscribe
   * @tparam _Callback Callback object to subscribe
   * @param _event Event object to subscribe
   * @param _callback Callback type object to subscribe
   */
  template<typename _Event,
           typename _Callback>
  void subscribe(_Event && _event, _Callback && _callback) {
    ProcessBus::getBus().subscribe(this, service_name_,
                                   std::forward<_Event>(_event),
                                   std::forward<_Callback>(_callback));
  };

  /**
   * This method is used to unsubscribe on event from IService<>
   * @tparam _Event Event object to unsubscribe
   * @tparam _Callback Callback object to unsubscribe
   * @param _event Event object to unsubscribe
   * @param _callback Callback type object to unsubscribe
   */
  template<typename _Event,
           typename _Callback>
  void unsubscribe(_Event && _event, _Callback && _callback) {
    ProcessBus::getBus().unsubscribe(this, service_name_,
                                     std::forward<_Event>(_event),
                                     std::forward<_Callback>(_callback));
  };

 private:
  const std::string service_name_;
};

/**
 * Destructor which disassemble the client
 */
template<typename _Interface>
inline
IClient<_Interface>::~IClient() {
  ProcessBus::getBus().disassembleClient(this, service_name_);
}

}

}

#endif //ICC_ISERVICECLIENT_HPP
