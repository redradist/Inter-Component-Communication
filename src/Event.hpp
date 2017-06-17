/**
 * @file Event.hpp
 * @author Denis Kotov
 * @date 10 Jun 2017
 * @brief Contains abstract class for Pack Buffer
 * @copyright MIT License. Open source:
 */

#ifndef ICC_EVENT_HPP
#define ICC_EVENT_HPP

#include <vector>
#include <map>
#include <tuple>
#include <utility>
#include "Component.hpp"

template <typename _T>
class Event;

template <typename _R, typename ... _Args>
class Event<_R(_Args...)> {
 public:
  using tCallback = std::function<void(_Args...)>;
  using tListCallbacks = std::vector<tCallback>;
  using tPairObjectAndCallbacks = std::pair<std::weak_ptr<IComponent>, tCallback>;
  using tCheckedListCallbacks = std::vector<tPairObjectAndCallbacks>;
 public:
  Event()
      : listeners_(std::make_shared<tListCallbacks>()),
        checked_listeners_(std::make_shared<tCheckedListCallbacks>()) {
  }
  Event(Event const&) = default;
  Event(Event &&) = default;

 public:
  /**
   * Unsafe function.
   * User should be confident that _listener lives at moment of calling callback
   * @tparam _Component
   * @param _callback
   * @param _listener
   */
  template <typename _Component>
  void connect(void(_Component::*_callback)(_Args...),
               _Component * _listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      listeners_->push_back([=](_Args... args){
        _listener->push([=]() mutable {
          (_listener->*_callback)(std::forward<_Args>(args)...);
        });
      });
    }
  }

  /**
   *
   * @tparam _Component
   * @param _callback
   * @param _listener
   */
  template <typename _Component>
  void connect(void(_Component::*_callback)(_Args...),
               std::shared_ptr<_Component> _listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    std::weak_ptr<_Component> _weak_listener = _listener;
    if (!_weak_listener.expired()) {
      checked_listeners_->emplace_back(_weak_listener, [=](_Args... args) {
        if (auto _observer = _weak_listener.lock()) {
          _observer->push([=, pointer = _observer.get()]() mutable {
            (pointer->*_callback)(std::forward<_Args>(args)...);
          });
        }
      });
    }
  }

  void operator()(_Args... _args) {
    for (auto & listener : *listeners_) {
      listener(_args...);
    }
    for (auto & listener : *checked_listeners_) {
      if (auto _observer = listener.first.lock()) {
        listener.second(_args...);
      } else {
        // Delete it
      }
    }
  }

  void operator()(_Args... _args) const {
    for (auto & listener : *listeners_) {
      listener(_args...);
    }
    for (auto & listener : *checked_listeners_) {
      if (auto _observer = listener.first.lock()) {
        listener.second(_args...);
      } else {
        // Delete it
      }
    }
  }

 private:
  std::shared_ptr<tListCallbacks> listeners_;
  std::shared_ptr<tCheckedListCallbacks> checked_listeners_;
};

#endif //ICC_EVENT_HPP
