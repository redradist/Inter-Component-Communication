/**
 * @file Event.hpp
 * @author Denis Kotov
 * @date 10 Jun 2017
 * @brief Contains abstract class for Pack Buffer
 * @copyright MIT License. Open source: https://github.com/redradist/Transport_Buffers.git
 */

#ifndef ICC_EVENT_HPP
#define ICC_EVENT_HPP

#include <vector>
#include <utility>
#include "Component.hpp"

template <typename _T>
class Event;

template <typename _R, typename ... _Args>
class Event<_R(_Args...)> {
 public:
  using tCallback = std::function<void(_Args...)>;
  using tListCallbacks = std::vector<tCallback>;
 public:
  Event()
      : listeners_(std::make_shared<tListCallbacks>()) {
  }
  Event(Event const&) = default;
  Event(Event &&) = default;

 public:
  /**
   *
   * @tparam _Component
   * @param Callback
   * @param _listener
   */
  template <typename _Component>
  void connect(void(_Component::*Callback)(_Args...), _Component * _listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      listeners_->push_back([=](_Args... args){
        _listener->push([=]() mutable {
          _listener->Callback(std::forward<_Args>(args)...);
        });
      });
    }
  }

  void operator()(_Args... _args) {
    for (auto & listener : *listeners_) {
      listener(_args...);
    }
  }

  void operator()(_Args... _args) const {
    for (auto & listener : *listeners_) {
      listener(_args...);
    }
  }

 private:
  std::shared_ptr<tListCallbacks> listeners_;
};

#endif //ICC_EVENT_HPP
