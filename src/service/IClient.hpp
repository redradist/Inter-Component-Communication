/**
 * @file IClient.hpp
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
  }

  void buildClient() {
    ProcessBus::getBus().buildClient(this->shared_from_this(), service_name_);
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
   * @param _callback Pointer to external method without parameters
   */
  template<typename _Callback>
  void call(_Callback && _callback) {
    ProcessBus::getBus().call(service_name_,
                              std::forward<_Callback>(_callback));
  };

  /**
   * This method is used to call method from IService<>
   * @tparam _Callback Type of callback functor
   * @tparam _Value0 Value #0 type those is passed to external method
   * @param _callback Pointer to external method
   * @param _value0 Argument #0 passed into external method
   */
  template<typename _Callback,
           typename _Value0>
  void call(_Callback && _callback, _Value0 && _value0) {
    ProcessBus::getBus().call(service_name_,
                              std::forward<_Callback>(_callback),
                              std::forward<_Value0>(_value0));
  };

  /**
   *
   * @tparam _Callback Type of callback functor
   * @tparam _Value0 Value #0 type those is passed to external method
   * @tparam _Value1 Value #1 type those is passed to external method
   * @param _callback Pointer to external method
   * @param _value0 Argument #0 passed into external method
   * @param _value1 Argument #1 passed into external method
   */
  template<typename _Callback,
           typename _Value0,
           typename _Value1>
  void call(_Callback && _callback, _Value0 && _value0, _Value1 && _value1) {
    ProcessBus::getBus().call(service_name_,
                              std::forward<_Callback>(_callback),
                              std::forward<_Value0>(_value0),
                              std::forward<_Value0>(_value1));
  };

  /**
   *
   * @tparam _Callback Type of callback functor
   * @tparam _Value0 Value #0 type those is passed to external method
   * @tparam _Value1 Value #1 type those is passed to external method
   * @tparam _Value2 Value #2 type those is passed to external method
   * @param _callback Pointer to external method
   * @param _value0 Argument #0 passed into external method
   * @param _value1 Argument #1 passed into external method
   * @param _value2 Argument #2 passed into external method
   */
  template<typename _Callback,
           typename _Value0,
           typename _Value1,
           typename _Value2>
  void call(_Callback && _callback, _Value0 && _value0, _Value1 && _value1, _Value2 && _value2) {
    ProcessBus::getBus().call(service_name_,
                              std::forward<_Callback>(_callback),
                              std::forward<_Value0>(_value0),
                              std::forward<_Value0>(_value1),
                              std::forward<_Value0>(_value2));
  };

  /**
   *
   * @tparam _Callback Type of callback functor
   * @tparam _Value0 Value #0 type those is passed to external method
   * @tparam _Value1 Value #1 type those is passed to external method
   * @tparam _Value2 Value #2 type those is passed to external method
   * @tparam _Value3 Value #3 type those is passed to external method
   * @param _callback Pointer to external method
   * @param _value0 Argument #0 passed into external method
   * @param _value1 Argument #1 passed into external method
   * @param _value2 Argument #2 passed into external method
   * @param _value3 Argument #3 passed into external method
   */
  template<typename _Callback,
           typename _Value0,
           typename _Value1,
           typename _Value2,
           typename _Value3>
  void call(_Callback && _callback, _Value0 && _value0, _Value1 && _value1, _Value2 && _value2, _Value3 && _value3) {
    ProcessBus::getBus().call(service_name_,
                              std::forward<_Callback>(_callback),
                              std::forward<_Value0>(_value0),
                              std::forward<_Value0>(_value1),
                              std::forward<_Value0>(_value2),
                              std::forward<_Value0>(_value3));
  };

  /**
   *
   * @tparam _Callback Type of callback functor
   * @tparam _Value0 Value #0 type those is passed to external method
   * @tparam _Value1 Value #1 type those is passed to external method
   * @tparam _Value2 Value #2 type those is passed to external method
   * @tparam _Value3 Value #3 type those is passed to external method
   * @tparam _Value4 Value #4 type those is passed to external method
   * @param _callback Pointer to external method
   * @param _value0 Argument #0 passed into external method
   * @param _value1 Argument #1 passed into external method
   * @param _value2 Argument #2 passed into external method
   * @param _value3 Argument #3 passed into external method
   * @param _value4 Argument #4 passed into external method
   */
  template<typename _Callback,
           typename _Value0,
           typename _Value1,
           typename _Value2,
           typename _Value3,
           typename _Value4>
  void call(_Callback && _callback, _Value0 && _value0, _Value1 && _value1, _Value2 && _value2, _Value3 && _value3, _Value4 && _value4) {
    ProcessBus::getBus().call(service_name_,
                              std::forward<_Callback>(_callback),
                              std::forward<_Value0>(_value0),
                              std::forward<_Value0>(_value1),
                              std::forward<_Value0>(_value2),
                              std::forward<_Value0>(_value3),
                              std::forward<_Value0>(_value4));
  };

  /**
   *
   * @tparam _Callback Type of callback functor
   * @tparam _Value0 Value #0 type those is passed to external method
   * @tparam _Value1 Value #1 type those is passed to external method
   * @tparam _Value2 Value #2 type those is passed to external method
   * @tparam _Value3 Value #3 type those is passed to external method
   * @tparam _Value4 Value #4 type those is passed to external method
   * @tparam _Value5 Value #5 type those is passed to external method
   * @param _callback Pointer to external method
   * @param _value0 Argument #0 passed into external method
   * @param _value1 Argument #1 passed into external method
   * @param _value2 Argument #2 passed into external method
   * @param _value3 Argument #3 passed into external method
   * @param _value4 Argument #4 passed into external method
   * @param _value5 Argument #5 passed into external method
   */
  template<typename _Callback,
           typename _Value0,
           typename _Value1,
           typename _Value2,
           typename _Value3,
           typename _Value4,
           typename _Value5>
  void call(_Callback && _callback, _Value0 && _value0, _Value1 && _value1, _Value2 && _value2, _Value3 && _value3, _Value4 && _value4, _Value5 && _value5) {
    ProcessBus::getBus().call(service_name_,
                              std::forward<_Callback>(_callback),
                              std::forward<_Value0>(_value0),
                              std::forward<_Value0>(_value1),
                              std::forward<_Value0>(_value2),
                              std::forward<_Value0>(_value3),
                              std::forward<_Value0>(_value4),
                              std::forward<_Value0>(_value5));
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
    ProcessBus::getBus().subscribe(this->shared_from_this(), service_name_,
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
    ProcessBus::getBus().unsubscribe(this->shared_from_this(), service_name_,
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
