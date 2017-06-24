/**
 * @file Event.hpp
 * @author Denis Kotov
 * @date 10 Jun 2017
 * @brief Contains Event class.
 * It is thread safe version of class like boost::signal and boost::signal2.
 * Safety guarantee on client side
 * @copyright MIT License. Open source:
 */

#ifndef ICC_EVENT_HPP
#define ICC_EVENT_HPP

#include <vector>
#include <map>
#include <tuple>
#include <utility>
#include <algorithm>
#include "IComponent.hpp"

template <typename _T>
class Event;

template <typename _R, typename ... _Args>
class Event<_R(_Args...)> {
 public:
  using tCallback = _R(IComponent::*)(_Args...);
  using tUncheckedObjectAndCallbacks = std::pair<IComponent *, tCallback>;
  using tUncheckedListCallbacks = std::vector<tUncheckedObjectAndCallbacks>;
  using tCheckedObjectAndCallbacks = std::pair<std::weak_ptr<IComponent>, tCallback>;
  using tCheckedListCallbacks = std::vector<tCheckedObjectAndCallbacks>;
 public:
  Event() = default;
  Event(Event const&) = default;
  Event(Event &&) = default;

 public:
  /**
   * Unsafe function for connect Event to object _listener with _callback
   * User should be confident that _listener is exist at moment of calling callback
   * @tparam _Component Type of object that listen Event
   * @param _callback method in object that listen Event
   * @param _listener Object that listen Event
   */
  template <typename _Component>
  void connect(_R(_Component::*_callback)(_Args...),
               _Component * _listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      unchecked_listeners_.emplace_back(
          static_cast<IComponent*>(_listener),
          static_cast<_R(IComponent::*)(_Args...)>(_callback));
    }
  }

  /**
   * Safe function for connect Event to object _listener with _callback
   * User can check if _listener is exist at moment of calling callback
   * @tparam _Component Type of object that listen Event
   * @param _callback method in object that listen Event
   * @param _listener Object that listen Event
   */
  template <typename _Component>
  void connect(_R(_Component::*_callback)(_Args...),
               std::shared_ptr<_Component> _listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if(_listener) {
      checked_listeners_.emplace_back(
          static_cast<std::shared_ptr<IComponent>>(_listener),
          static_cast<_R(IComponent::*)(_Args...)>(_callback)
      );
    }
  }

  /**
   * Unsafe function for disconnect Event from object _listener with _callback
   * User should be confident that _listener is exist at moment of calling callback
   * @tparam _Component Type of object that listen Event
   * @param _callback method in object that listen Event
   * @param _listener Object that listen Event
   */
  template <typename _Component>
  void disconnect(_R(_Component::*_callback)(_Args...),
                  _Component * _listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      std::pair<IComponent *, tCallback> removedCallback = {
          static_cast<IComponent*>(_listener),
          static_cast<_R(IComponent::*)(_Args...)>(_callback)
        };

      auto erase = std::remove(unchecked_listeners_.begin(),
                               unchecked_listeners_.end(),
                               removedCallback);
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
  template <typename _Component>
  void disconnect(_R(_Component::*_callback)(_Args...),
                  std::shared_ptr<_Component> _listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if(_listener) {
      std::pair<std::weak_ptr<IComponent>, tCallback> removedCallback = {
          static_cast<std::shared_ptr<IComponent>>(_listener),
          static_cast<_R(IComponent::*)(_Args...)>(_callback)
      };
      auto erase = std::remove_if(checked_listeners_.begin(),
                                  checked_listeners_.end(),
      [=](const std::pair<std::weak_ptr<IComponent>, tCallback> & rad) {
        bool result = false;
        if (auto _observer = rad.first.lock()) {
          result = (_callback == static_cast<void(_Component::*)(_Args...)>(rad.second));
        } else {
          result = true;
        }
        return result;
      });
      checked_listeners_.erase(erase, checked_listeners_.end());
    }
  }

  /**
   * Method for calling Event
   * @param _args Parameters for calling Event
   */
  void operator()(_Args... _args) {
    for (auto & listener : unchecked_listeners_) {
      (listener.first)->push([=]() mutable {
        ((listener.first)->*(listener.second))(std::forward<_Args>(_args)...);
      });
    }
    for (auto listener = checked_listeners_.begin();
         listener != checked_listeners_.end();) {
      if (auto _observer = listener->first.lock()) {
        _observer->push([=]() mutable {
          ((_observer.get())->*(listener->second))(std::forward<_Args>(_args)...);
        });
        ++listener;
      } else {
        // NOTE(redra): Deleting listener
        listener = checked_listeners_.erase(listener);
      }
    }
  }

  /**
   * Method for calling const Event
   * @param _args Parameters for calling const Event
   */
  void operator()(_Args... _args) const {
    for (auto & listener : unchecked_listeners_) {
      (listener.first)->push([=]() mutable {
        ((listener.first)->*(listener.second))(std::forward<_Args>(_args)...);
      });
    }
    for (auto listener : checked_listeners_) {
      if (auto _observer = listener.first.lock()) {
        _observer->push([=]() mutable {
          ((_observer.get())->*(listener.second))(std::forward<_Args>(_args)...);
        });
      }
    }
  }

  /**
   * Convertion function is used to convert Event to std::function
   * This function is used only for checked listeners because of safety
   * @return std::function object
   */
  operator std::function<_R(_Args...)>() {
    return [event = *this](_Args... _args) mutable {
      for (auto listener = event.checked_listeners_.begin();
           listener != event.checked_listeners_.end();) {
        if (auto _observer = listener->first.lock()) {
          _observer->push([=]() mutable {
            ((_observer.get())->*(listener->second))(std::forward<_Args>(_args)...);
          });
          ++listener;
        } else {
          // NOTE(redra): Deleting listener
          listener = event.checked_listeners_.erase(listener);
        }
      }
      return _R();
    };
  }

 private:
  tUncheckedListCallbacks unchecked_listeners_;
  tCheckedListCallbacks checked_listeners_;
};

#endif //ICC_EVENT_HPP
