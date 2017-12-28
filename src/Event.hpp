/**
 * @file Event.hpp
 * @author Denis Kotov
 * @date 10 Jun 2017
 * @brief Contains Event class.
 * It is thread safe version of class like boost::signal and boost::signal2.
 * Safety guarantee on client side
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_EVENT_HPP
#define ICC_EVENT_HPP

#include <vector>
#include <map>
#include <tuple>
#include <utility>
#include <algorithm>
#include <mutex>
#include <helpers/memory_helpers.hpp>
#include "IComponent.hpp"

namespace icc {

template<typename _T>
class Event;

template<typename _R, typename ... _Args>
class Event<_R(_Args...)> {
 public:
  using tPointer = void *;
  using tCallback = std::function<_R(_Args...)>;
  using tUncheckedCallbacks = std::tuple<IComponent *, tPointer, tCallback>;
  using tUncheckedListCallbacks = std::vector<tUncheckedCallbacks>;
  using tCheckedCallbacks = std::tuple<std::weak_ptr<IComponent>, tPointer, tCallback>;
  using tCheckedListCallbacks = std::vector<tCheckedCallbacks>;
 public:
  Event() = default;
  /**
   * Specific copy constructor.
   * Disable ability to copy unchecked listeners as unsafe.
   * @param _event Event from which we copy listeners
   */
  Event(Event const &_event)
      : checked_listeners_(_event.checked_listeners_) {
  }
  Event(Event &&) = delete;

 public:
  /**
   * Unsafe function for connect Event to object _listener with _callback
   * User should be confident that _listener is exist at moment of calling callback
   * @tparam _Component Type of object that listen Event
   * @param _callback method in object that listen Event
   * @param _listener Object that listen Event
   */
  template<typename _Component>
  void connect(_R(_Component::*_callback)(_Args...),
               _Component *_listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      std::lock_guard<std::mutex> lock(mutex_);
      tUncheckedCallbacks callback(
          static_cast<IComponent *>(_listener),
          icc::helpers::void_cast(_callback),
          [=](_Args ... _args) {
            (_listener->*_callback)(_args...);
          });
      unchecked_listeners_.push_back(callback);
    }
  }

  /**
   * Safe function for connect Event to object _listener with _callback
   * User can check if _listener is exist at moment of calling callback
   * @tparam _Component Type of object that listen Event
   * @param _callback method in object that listen Event
   * @param _listener Object that listen Event
   */
  template<typename _Component>
  void connect(_R(_Component::*_callback)(_Args...),
               std::shared_ptr<_Component> _listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      std::lock_guard<std::mutex> lock(mutex_);
      auto _p_listener = _listener.get();
      tCheckedCallbacks callback(
          std::static_pointer_cast<IComponent>(_listener),
          icc::helpers::void_cast(_callback),
          [=](_Args ... _args) {
            (_p_listener->*_callback)(_args...);
          });
      checked_listeners_.push_back(callback);
    }
  }

  /**
   * Unsafe function for disconnect Event from object _listener with _callback
   * User should be confident that _listener is exist at moment of calling callback
   * @tparam _Component Type of object that listen Event
   * @param _callback method in object that listen Event
   * @param _listener Object that listen Event
   */
  template<typename _Component>
  void disconnect(_R(_Component::*_callback)(_Args...),
                  _Component *_listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      std::lock_guard<std::mutex> lock(mutex_);
      auto erase = std::remove_if(unchecked_listeners_.begin(),
                                  unchecked_listeners_.end(),
                                  [=](const tUncheckedCallbacks &rad) {
                                    return (_listener == std::get<0>(rad)) &&
                                        (icc::helpers::void_cast(_callback) == std::get<1>(rad));
                                  });
      unchecked_listeners_.erase(erase, unchecked_listeners_.end());
    }
  }

  /**
   * Safe function for disconnect Event from object _listener with _callback
   * User can check if _listener is exist at moment of calling callback
   * @tparam _Component Type of object that listen Event
   * @param _callback method in object that listen Event
   * @param _listener Object that listen Event
   */
  template<typename _Component>
  void disconnect(_R(_Component::*_callback)(_Args...),
                  std::shared_ptr<_Component> _listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      std::lock_guard<std::mutex> lock(mutex_);
      auto erase = std::remove_if(checked_listeners_.begin(),
                                  checked_listeners_.end(),
                                  [=](const tCheckedCallbacks &rad) {
                                    bool result = false;
                                    if (auto _observer = std::get<0>(rad).lock()) {
                                      result = (_observer == _listener) &&
                                          (icc::helpers::void_cast(_callback) == std::get<1>(rad));
                                    } else {
                                      result = true;
                                    }
                                    return result;
                                  });
      checked_listeners_.erase(erase, checked_listeners_.end());
    }
  }

  /**
   * Used to disconnect all clients from this event
   */
  void disconnectAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    unchecked_listeners_.clear();
    checked_listeners_.clear();
  }

  /**
   * Method for calling Event
   * @param _args Parameters for calling Event
   */
  void operator()(_Args ... _args) {
    tUncheckedListCallbacks uncheckedListeners;
    tCheckedListCallbacks checkedListeners;
    copyClients(uncheckedListeners, checkedListeners);
    for (auto &listener : uncheckedListeners) {
      auto client = std::get<0>(listener);
      auto callback = std::get<2>(listener);
      client->push([=]() mutable {
        callback(_args...);
      });
    }
    for (auto &listener : checkedListeners) {
      auto client = std::get<0>(listener);
      if (auto _observer = client.lock()) {
        auto callback = std::get<2>(listener);
        _observer->push([=]() mutable {
          if (auto _observer = client.lock()) {
            callback(_args...);
          }
        });
      }
    }
    clearExpiredClient();
  }

  /**
   * Method for calling const Event
   * @param _args Parameters for calling const Event
   */
  void operator()(_Args ... _args) const {
    for (auto &listener : unchecked_listeners_) {
      auto client = std::get<0>(listener);
      auto callback = std::get<2>(listener);
      client->push([=]() mutable {
        callback(_args...);
      });
    }
    for (auto &listener : checked_listeners_) {
      auto client = std::get<0>(listener);
      if (auto _observer = client.lock()) {
        auto callback = std::get<2>(listener);
        _observer->push([=]() mutable {
          if (auto _observer = client.lock()) {
            callback(_args...);
          }
        });
      }
    }
  }

  /**
   * Conversion function is used to convert Event to std::function
   * This function is used only for checked listeners because of safety
   * @return std::function object
   */
  operator std::function<_R(_Args...)>() {
    std::lock_guard<std::mutex> lock(mutex_);
    auto event = *this;
    return [event](_Args ... _args) mutable {
      tUncheckedListCallbacks _;
      tCheckedListCallbacks checkedListeners;
      event.copyClients(_, checkedListeners);
      for (auto &listener : checkedListeners) {
        auto client = std::get<0>(listener);
        if (auto _observer = client.lock()) {
          auto callback = std::get<2>(listener);
          _observer->push([=]() mutable {
            if (auto _observer = client.lock()) {
              callback(_args...);
            }
          });
        }
      }
      event.clearExpiredClient();
      return _R();
    };
  }

  void copyClients(tUncheckedListCallbacks & _uncheckedListeners,
                   tCheckedListCallbacks & _checkedListeners) {
    std::lock_guard<std::mutex> lock(mutex_);
    _uncheckedListeners = unchecked_listeners_;
    _checkedListeners = checked_listeners_;
  }

  void clearExpiredClient() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto listener = checked_listeners_.begin();
         listener != checked_listeners_.end();) {
      if (std::get<0>(*listener).expired()) {
        // NOTE(redra): Deleting listener
        listener = checked_listeners_.erase(listener);
      } else {
        ++listener;
      }
    }
  }

 private:
  std::mutex mutex_;
  tUncheckedListCallbacks unchecked_listeners_;
  tCheckedListCallbacks checked_listeners_;
};

}

#endif //ICC_EVENT_HPP
