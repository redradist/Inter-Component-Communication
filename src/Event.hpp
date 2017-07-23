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
#include <helpers/memory_helpers.hpp>
#include "IComponent.hpp"

namespace icc {

template<typename _T>
class Event;

template<typename _R>
class Event<_R(void)> {
 public:
  using tPointer = void *;
  using tCallback = std::function<_R(void)>;
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
  void connect(_R(_Component::*_callback)(void),
               _Component *_listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      tUncheckedCallbacks callback(
          static_cast<IComponent *>(_listener),
          std::void_cast(_callback),
          [=]() {
            (_listener->*_callback)();
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
  void connect(_R(_Component::*_callback)(void),
               std::shared_ptr<_Component> _listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      auto _p_listener = _listener.get();
      tCheckedCallbacks callback(
          std::static_pointer_cast<IComponent>(_listener),
          std::void_cast(_callback),
          [=]() {
            (_p_listener->*_callback)();
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
  void disconnect(_R(_Component::*_callback)(void),
                  _Component *_listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      auto erase = std::remove_if(unchecked_listeners_.begin(),
                                  unchecked_listeners_.end(),
                                  [=](const tUncheckedCallbacks &rad) {
                                    return (_listener == std::get<0>(rad)) &&
                                        (std::void_cast(_callback) == std::get<1>(rad));
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
  void disconnect(_R(_Component::*_callback)(void),
                  std::shared_ptr<_Component> _listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      auto erase = std::remove_if(checked_listeners_.begin(),
                                  checked_listeners_.end(),
                                  [=](const tCheckedCallbacks &rad) {
                                    bool result = false;
                                    if (auto _observer = std::get<0>(rad).lock()) {
                                      result = (_observer == _listener) &&
                                          (std::void_cast(_callback) == std::get<1>(rad));
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
    unchecked_listeners_.clear();
    checked_listeners_.clear();
  }

  /**
   * Method for calling Event
   */
  void operator()() {
    for (auto &listener : unchecked_listeners_) {
      auto client = std::get<0>(listener);
      auto callback = std::get<2>(listener);
      client->send([=]() mutable {
        callback();
      });
    }
    for (auto listener = checked_listeners_.begin();
         listener != checked_listeners_.end();) {
      auto client = std::get<0>(*listener);
      if (auto _observer = client.lock()) {
        auto callback = std::get<2>(*listener);
        _observer->send([=]() mutable {
          callback();
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
   */
  void operator()() const {
    for (auto &listener : unchecked_listeners_) {
      auto client = std::get<0>(listener);
      auto callback = std::get<2>(listener);
      client->send([=]() mutable {
        callback();
      });
    }
    for (auto listener : checked_listeners_) {
      auto client = std::get<0>(*listener);
      if (auto _observer = client.lock()) {
        auto callback = std::get<2>(*listener);
        _observer->send([=]() mutable {
          callback();
        });
      }
    }
  }

  /**
   * Convertion function is used to convert Event to std::function
   * This function is used only for checked listeners because of safety
   * @return std::function object
   */
  operator std::function<_R(void)>() {
    auto event = *this;
    return [event]() mutable {
      for (auto listener = event.checked_listeners_.begin();
           listener != event.checked_listeners_.end();) {
        if (auto _observer = (std::get<0>(*listener)).lock()) {
          auto callback = std::get<2>(*listener);
          _observer->send([=]() mutable {
            callback();
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

template<typename _R, typename _Arg0>
class Event<_R(_Arg0)> {
 public:
  using tPointer = void *;
  using tCallback = std::function<_R(_Arg0)>;
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
  void connect(_R(_Component::*_callback)(_Arg0),
               _Component *_listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      tUncheckedCallbacks callback(
          static_cast<IComponent *>(_listener),
          std::void_cast(_callback),
          [=](_Arg0 _arg0) {
            (_listener->*_callback)(_arg0);
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
  void connect(_R(_Component::*_callback)(_Arg0),
               std::shared_ptr<_Component> _listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      auto _p_listener = _listener.get();
      tCheckedCallbacks callback(
          std::static_pointer_cast<IComponent>(_listener),
          std::void_cast(_callback),
          [=](_Arg0 _arg0) {
            (_p_listener->*_callback)(_arg0);
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
  void disconnect(_R(_Component::*_callback)(_Arg0),
                  _Component *_listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      auto erase = std::remove_if(unchecked_listeners_.begin(),
                                  unchecked_listeners_.end(),
                                  [=](const tUncheckedCallbacks &rad) {
                                    return (_listener == std::get<0>(rad)) &&
                                        (std::void_cast(_callback) == std::get<1>(rad));
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
  void disconnect(_R(_Component::*_callback)(_Arg0),
                  std::shared_ptr<_Component> _listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      auto erase = std::remove_if(checked_listeners_.begin(),
                                  checked_listeners_.end(),
                                  [=](const tCheckedCallbacks &rad) {
                                    bool result = false;
                                    if (auto _observer = std::get<0>(rad).lock()) {
                                      result = (_observer == _listener) &&
                                          (std::void_cast(_callback) == std::get<1>(rad));
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
    unchecked_listeners_.clear();
    checked_listeners_.clear();
  }

  /**
   * Method for calling Event
   * @param _arg0 Parameter #0 for calling Event
   */
  void operator()(_Arg0 _arg0) {
    for (auto &listener : unchecked_listeners_) {
      auto client = std::get<0>(listener);
      auto callback = std::get<2>(listener);
      client->send([=]() mutable {
        callback(_arg0);
      });
    }
    for (auto listener = checked_listeners_.begin();
         listener != checked_listeners_.end();) {
      auto client = std::get<0>(*listener);
      if (auto _observer = client.lock()) {
        auto callback = std::get<2>(*listener);
        _observer->send([=]() mutable {
          callback(_arg0);
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
   * @param _arg0 Parameter #0 for calling const Event
   */
  void operator()(_Arg0 _arg0) const {
    for (auto &listener : unchecked_listeners_) {
      auto client = std::get<0>(listener);
      auto callback = std::get<2>(listener);
      client->send([=]() mutable {
        callback(_arg0);
      });
    }
    for (auto listener : checked_listeners_) {
      auto client = std::get<0>(*listener);
      if (auto _observer = client.lock()) {
        auto callback = std::get<2>(*listener);
        _observer->send([=]() mutable {
          callback(_arg0);
        });
      }
    }
  }

  /**
   * Convertion function is used to convert Event to std::function
   * This function is used only for checked listeners because of safety
   * @return std::function object
   */
  operator std::function<_R(_Arg0)>() {
    auto event = *this;
    return [event](_Arg0 _arg0) mutable {
      for (auto listener = event.checked_listeners_.begin();
           listener != event.checked_listeners_.end();) {
        if (auto _observer = (std::get<0>(*listener)).lock()) {
          auto callback = std::get<2>(*listener);
          _observer->send([=]() mutable {
            callback(_arg0);
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

template<typename _R, typename _Arg0,
                      typename _Arg1>
class Event<_R(_Arg0, _Arg1)> {
 public:
  using tPointer = void *;
  using tCallback = std::function<_R(_Arg0, _Arg1)>;
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
  void connect(_R(_Component::*_callback)(_Arg0, _Arg1),
               _Component *_listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      tUncheckedCallbacks callback(
          static_cast<IComponent *>(_listener),
          std::void_cast(_callback),
          [=](_Arg0 _arg0, _Arg1 _arg1) {
            (_listener->*_callback)(_arg0, _arg1);
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
  void connect(_R(_Component::*_callback)(_Arg0, _Arg1),
               std::shared_ptr<_Component> _listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      auto _p_listener = _listener.get();
      tCheckedCallbacks callback(
          std::static_pointer_cast<IComponent>(_listener),
          std::void_cast(_callback),
          [=](_Arg0 _arg0, _Arg1 _arg1) {
            (_p_listener->*_callback)(_arg0, _arg1);
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
  void disconnect(_R(_Component::*_callback)(_Arg0, _Arg1),
                  _Component *_listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      auto erase = std::remove_if(unchecked_listeners_.begin(),
                                  unchecked_listeners_.end(),
                                  [=](const tUncheckedCallbacks &rad) {
                                    return (_listener == std::get<0>(rad)) &&
                                        (std::void_cast(_callback) == std::get<1>(rad));
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
  void disconnect(_R(_Component::*_callback)(_Arg0, _Arg1),
                  std::shared_ptr<_Component> _listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      auto erase = std::remove_if(checked_listeners_.begin(),
                                  checked_listeners_.end(),
                                  [=](const tCheckedCallbacks &rad) {
                                    bool result = false;
                                    if (auto _observer = std::get<0>(rad).lock()) {
                                      result = (_observer == _listener) &&
                                          (std::void_cast(_callback) == std::get<1>(rad));
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
    unchecked_listeners_.clear();
    checked_listeners_.clear();
  }

  /**
   * Method for calling Event
   * @param _arg0 Parameter #0 for calling Event
   * @param _arg1 Parameter #1 for calling Event
   */
  void operator()(_Arg0 _arg0, _Arg1 _arg1) {
    for (auto &listener : unchecked_listeners_) {
      auto client = std::get<0>(listener);
      auto callback = std::get<2>(listener);
      client->send([=]() mutable {
        callback(_arg0, _arg1);
      });
    }
    for (auto listener = checked_listeners_.begin();
         listener != checked_listeners_.end();) {
      auto client = std::get<0>(*listener);
      if (auto _observer = client.lock()) {
        auto callback = std::get<2>(*listener);
        _observer->send([=]() mutable {
          callback(_arg0, _arg1);
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
   * @param _arg0 Parameter #0 for calling Event
   * @param _arg1 Parameter #1 for calling Event
   */
  void operator()(_Arg0 _arg0, _Arg1 _arg1) const {
    for (auto &listener : unchecked_listeners_) {
      auto client = std::get<0>(listener);
      auto callback = std::get<2>(listener);
      client->send([=]() mutable {
        callback(_arg0, _arg1);
      });
    }
    for (auto listener : checked_listeners_) {
      auto client = std::get<0>(*listener);
      if (auto _observer = client.lock()) {
        auto callback = std::get<2>(*listener);
        _observer->send([=]() mutable {
          callback(_arg0, _arg1);
        });
      }
    }
  }

  /**
   * Convertion function is used to convert Event to std::function
   * This function is used only for checked listeners because of safety
   * @return std::function object
   */
  operator std::function<_R(_Arg0, _Arg1)>() {
    auto event = *this;
    return [event](_Arg0 _arg0, _Arg1 _arg1) mutable {
      for (auto listener = event.checked_listeners_.begin();
           listener != event.checked_listeners_.end();) {
        if (auto _observer = (std::get<0>(*listener)).lock()) {
          auto callback = std::get<2>(*listener);
          _observer->send([=]() mutable {
            callback(_arg0, _arg1);
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

template<typename _R, typename _Arg0,
                      typename _Arg1,
                      typename _Arg2>
class Event<_R(_Arg0, _Arg1, _Arg2)> {
 public:
  using tPointer = void *;
  using tCallback = std::function<_R(_Arg0, _Arg1, _Arg2)>;
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
  void connect(_R(_Component::*_callback)(_Arg0, _Arg1, _Arg2),
               _Component *_listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      tUncheckedCallbacks callback(
          static_cast<IComponent *>(_listener),
          std::void_cast(_callback),
          [=](_Arg0 _arg0, _Arg1 _arg1, _Arg2 _arg2) {
            (_listener->*_callback)(_arg0, _arg1, _arg2);
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
  void connect(_R(_Component::*_callback)(_Arg0, _Arg1, _Arg2),
               std::shared_ptr<_Component> _listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      auto _p_listener = _listener.get();
      tCheckedCallbacks callback(
          std::static_pointer_cast<IComponent>(_listener),
          std::void_cast(_callback),
          [=](_Arg0 _arg0, _Arg1 _arg1, _Arg2 _arg2) {
            (_p_listener->*_callback)(_arg0, _arg1, _arg2);
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
  void disconnect(_R(_Component::*_callback)(_Arg0, _Arg1, _Arg2),
                  _Component *_listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      auto erase = std::remove_if(unchecked_listeners_.begin(),
                                  unchecked_listeners_.end(),
                                  [=](const tUncheckedCallbacks &rad) {
                                    return (_listener == std::get<0>(rad)) &&
                                        (std::void_cast(_callback) == std::get<1>(rad));
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
  void disconnect(_R(_Component::*_callback)(_Arg0, _Arg1, _Arg2),
                  std::shared_ptr<_Component> _listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      auto erase = std::remove_if(checked_listeners_.begin(),
                                  checked_listeners_.end(),
                                  [=](const tCheckedCallbacks &rad) {
                                    bool result = false;
                                    if (auto _observer = std::get<0>(rad).lock()) {
                                      result = (_observer == _listener) &&
                                          (std::void_cast(_callback) == std::get<1>(rad));
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
    unchecked_listeners_.clear();
    checked_listeners_.clear();
  }

  /**
   * Method for calling Event
   * @param _arg0 Parameter #0 for calling Event
   * @param _arg1 Parameter #1 for calling Event
   * @param _arg2 Parameter #2 for calling Event
   */
  void operator()(_Arg0 _arg0, _Arg1 _arg1, _Arg2 _arg2) {
    for (auto &listener : unchecked_listeners_) {
      auto client = std::get<0>(listener);
      auto callback = std::get<2>(listener);
      client->send([=]() mutable {
        callback(_arg0, _arg1, _arg2);
      });
    }
    for (auto listener = checked_listeners_.begin();
         listener != checked_listeners_.end();) {
      auto client = std::get<0>(*listener);
      if (auto _observer = client.lock()) {
        auto callback = std::get<2>(*listener);
        _observer->send([=]() mutable {
          callback(_arg0, _arg1, _arg2);
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
   * @param _arg0 Parameter #0 for calling Event
   * @param _arg1 Parameter #1 for calling Event
   * @param _arg2 Parameter #2 for calling Event
   */
  void operator()(_Arg0 _arg0, _Arg1 _arg1, _Arg2 _arg2) const {
    for (auto &listener : unchecked_listeners_) {
      auto client = std::get<0>(listener);
      auto callback = std::get<2>(listener);
      client->send([=]() mutable {
        callback(_arg0, _arg1, _arg2);
      });
    }
    for (auto listener : checked_listeners_) {
      auto client = std::get<0>(*listener);
      if (auto _observer = client.lock()) {
        auto callback = std::get<2>(*listener);
        _observer->send([=]() mutable {
          callback(_arg0, _arg1, _arg2);
        });
      }
    }
  }

  /**
   * Convertion function is used to convert Event to std::function
   * This function is used only for checked listeners because of safety
   * @return std::function object
   */
  operator std::function<_R(_Arg0, _Arg1, _Arg2)>() {
    auto event = *this;
    return [event](_Arg0 _arg0, _Arg1 _arg1, _Arg2 _arg2) mutable {
      for (auto listener = event.checked_listeners_.begin();
           listener != event.checked_listeners_.end();) {
        if (auto _observer = (std::get<0>(*listener)).lock()) {
          auto callback = std::get<2>(*listener);
          _observer->send([=]() mutable {
            callback(_arg0, _arg1, _arg2);
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

template<typename _R, typename _Arg0,
                      typename _Arg1,
                      typename _Arg2,
                      typename _Arg3>
class Event<_R(_Arg0, _Arg1, _Arg2, _Arg3)> {
 public:
  using tPointer = void *;
  using tCallback = std::function<_R(_Arg0, _Arg1, _Arg2, _Arg3)>;
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
  void connect(_R(_Component::*_callback)(_Arg0, _Arg1, _Arg2, _Arg3),
               _Component *_listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      tUncheckedCallbacks callback(
          static_cast<IComponent *>(_listener),
          std::void_cast(_callback),
          [=](_Arg0 _arg0, _Arg1 _arg1, _Arg2 _arg2, _Arg3 _arg3) {
            (_listener->*_callback)(_arg0, _arg1, _arg2, _arg3);
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
  void connect(_R(_Component::*_callback)(_Arg0, _Arg1, _Arg2, _Arg3),
               std::shared_ptr<_Component> _listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      auto _p_listener = _listener.get();
      tCheckedCallbacks callback(
          std::static_pointer_cast<IComponent>(_listener),
          std::void_cast(_callback),
          [=](_Arg0 _arg0, _Arg1 _arg1, _Arg2 _arg2, _Arg3 _arg3) {
            (_p_listener->*_callback)(_arg0, _arg1, _arg2, _arg3);
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
  void disconnect(_R(_Component::*_callback)(_Arg0, _Arg1, _Arg2, _Arg3),
                  _Component *_listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      auto erase = std::remove_if(unchecked_listeners_.begin(),
                                  unchecked_listeners_.end(),
                                  [=](const tUncheckedCallbacks &rad) {
                                    return (_listener == std::get<0>(rad)) &&
                                        (std::void_cast(_callback) == std::get<1>(rad));
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
  void disconnect(_R(_Component::*_callback)(_Arg0, _Arg1, _Arg2, _Arg3),
                  std::shared_ptr<_Component> _listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      auto erase = std::remove_if(checked_listeners_.begin(),
                                  checked_listeners_.end(),
                                  [=](const tCheckedCallbacks &rad) {
                                    bool result = false;
                                    if (auto _observer = std::get<0>(rad).lock()) {
                                      result = (_observer == _listener) &&
                                          (std::void_cast(_callback) == std::get<1>(rad));
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
    unchecked_listeners_.clear();
    checked_listeners_.clear();
  }

  /**
   * Method for calling Event
   * @param _arg0 Parameter #0 for calling Event
   * @param _arg1 Parameter #1 for calling Event
   * @param _arg2 Parameter #2 for calling Event
   * @param _arg4 Parameter #3 for calling Event
   */
  void operator()(_Arg0 _arg0, _Arg1 _arg1, _Arg2 _arg2, _Arg3 _arg3) {
    for (auto &listener : unchecked_listeners_) {
      auto client = std::get<0>(listener);
      auto callback = std::get<2>(listener);
      client->send([=]() mutable {
        callback(_arg0, _arg1, _arg2, _arg3);
      });
    }
    for (auto listener = checked_listeners_.begin();
         listener != checked_listeners_.end();) {
      auto client = std::get<0>(*listener);
      if (auto _observer = client.lock()) {
        auto callback = std::get<2>(*listener);
        _observer->send([=]() mutable {
          callback(_arg0, _arg1, _arg2, _arg3);
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
   * @param _arg0 Parameter #0 for calling Event
   * @param _arg1 Parameter #1 for calling Event
   * @param _arg2 Parameter #2 for calling Event
   * @param _arg4 Parameter #3 for calling Event
   */
  void operator()(_Arg0 _arg0, _Arg1 _arg1, _Arg2 _arg2, _Arg3 _arg3) const {
    for (auto &listener : unchecked_listeners_) {
      auto client = std::get<0>(listener);
      auto callback = std::get<2>(listener);
      client->send([=]() mutable {
        callback(_arg0, _arg1, _arg2, _arg3);
      });
    }
    for (auto listener : checked_listeners_) {
      auto client = std::get<0>(*listener);
      if (auto _observer = client.lock()) {
        auto callback = std::get<2>(*listener);
        _observer->send([=]() mutable {
          callback(_arg0, _arg1, _arg2, _arg3);
        });
      }
    }
  }

  /**
   * Convertion function is used to convert Event to std::function
   * This function is used only for checked listeners because of safety
   * @return std::function object
   */
  operator std::function<_R(_Arg0, _Arg1, _Arg2, _Arg3)>() {
    auto event = *this;
    return [event](_Arg0 _arg0, _Arg1 _arg1, _Arg2 _arg2, _Arg3 _arg3) mutable {
      for (auto listener = event.checked_listeners_.begin();
           listener != event.checked_listeners_.end();) {
        if (auto _observer = (std::get<0>(*listener)).lock()) {
          auto callback = std::get<2>(*listener);
          _observer->send([=]() mutable {
            callback(_arg0, _arg1, _arg2, _arg3);
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

template<typename _R, typename _Arg0,
                      typename _Arg1,
                      typename _Arg2,
                      typename _Arg3,
                      typename _Arg4>
class Event<_R(_Arg0, _Arg1, _Arg2, _Arg3, _Arg4)> {
 public:
  using tPointer = void *;
  using tCallback = std::function<_R(_Arg0, _Arg1, _Arg2, _Arg3, _Arg4)>;
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
  void connect(_R(_Component::*_callback)(_Arg0, _Arg1, _Arg2, _Arg3, _Arg4),
               _Component *_listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      tUncheckedCallbacks callback(
          static_cast<IComponent *>(_listener),
          std::void_cast(_callback),
          [=](_Arg0 _arg0, _Arg1 _arg1, _Arg2 _arg2, _Arg3 _arg3, _Arg4 _arg4) {
            (_listener->*_callback)(_arg0, _arg1, _arg2, _arg3, _arg4);
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
  void connect(_R(_Component::*_callback)(_Arg0, _Arg1, _Arg2, _Arg3, _Arg4),
               std::shared_ptr<_Component> _listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      auto _p_listener = _listener.get();
      tCheckedCallbacks callback(
          std::static_pointer_cast<IComponent>(_listener),
          std::void_cast(_callback),
          [=](_Arg0 _arg0, _Arg1 _arg1, _Arg2 _arg2, _Arg3 _arg3, _Arg4 _arg4) {
            (_p_listener->*_callback)(_arg0, _arg1, _arg2, _arg3, _arg4);
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
  void disconnect(_R(_Component::*_callback)(_Arg0, _Arg1, _Arg2, _Arg3, _Arg4),
                  _Component *_listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      auto erase = std::remove_if(unchecked_listeners_.begin(),
                                  unchecked_listeners_.end(),
                                  [=](const tUncheckedCallbacks &rad) {
                                    return (_listener == std::get<0>(rad)) &&
                                        (std::void_cast(_callback) == std::get<1>(rad));
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
  void disconnect(_R(_Component::*_callback)(_Arg0, _Arg1, _Arg2, _Arg3, _Arg4),
                  std::shared_ptr<_Component> _listener) {
    static_assert(std::is_base_of<IComponent, _Component>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      auto erase = std::remove_if(checked_listeners_.begin(),
                                  checked_listeners_.end(),
                                  [=](const tCheckedCallbacks &rad) {
                                    bool result = false;
                                    if (auto _observer = std::get<0>(rad).lock()) {
                                      result = (_observer == _listener) &&
                                          (std::void_cast(_callback) == std::get<1>(rad));
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
    unchecked_listeners_.clear();
    checked_listeners_.clear();
  }

  /**
   * Method for calling Event
   * @param _arg0 Parameter #0 for calling Event
   * @param _arg1 Parameter #1 for calling Event
   * @param _arg2 Parameter #2 for calling Event
   * @param _arg4 Parameter #3 for calling Event
   * @param _arg5 Parameter #4 for calling Event
   */
  void operator()(_Arg0 _arg0, _Arg1 _arg1, _Arg2 _arg2, _Arg3 _arg3, _Arg4 _arg4) {
    for (auto &listener : unchecked_listeners_) {
      auto client = std::get<0>(listener);
      auto callback = std::get<2>(listener);
      client->send([=]() mutable {
        callback(_arg0, _arg1, _arg2, _arg3, _arg4);
      });
    }
    for (auto listener = checked_listeners_.begin();
         listener != checked_listeners_.end();) {
      auto client = std::get<0>(*listener);
      if (auto _observer = client.lock()) {
        auto callback = std::get<2>(*listener);
        _observer->send([=]() mutable {
          callback(_arg0, _arg1, _arg2, _arg3, _arg4);
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
   * @param _arg0 Parameter #0 for calling Event
   * @param _arg1 Parameter #1 for calling Event
   * @param _arg2 Parameter #2 for calling Event
   * @param _arg4 Parameter #3 for calling Event
   * @param _arg5 Parameter #4 for calling Event
   */
  void operator()(_Arg0 _arg0, _Arg1 _arg1, _Arg2 _arg2, _Arg3 _arg3, _Arg4 _arg4) const {
    for (auto &listener : unchecked_listeners_) {
      auto client = std::get<0>(listener);
      auto callback = std::get<2>(listener);
      client->send([=]() mutable {
        callback(_arg0, _arg1, _arg2, _arg3, _arg4);
      });
    }
    for (auto listener : checked_listeners_) {
      auto client = std::get<0>(*listener);
      if (auto _observer = client.lock()) {
        auto callback = std::get<2>(*listener);
        _observer->send([=]() mutable {
          callback(_arg0, _arg1, _arg2, _arg3, _arg4);
        });
      }
    }
  }

  /**
   * Convertion function is used to convert Event to std::function
   * This function is used only for checked listeners because of safety
   * @return std::function object
   */
  operator std::function<_R(_Arg0, _Arg1, _Arg2, _Arg3, _Arg4)>() {
    auto event = *this;
    return [event](_Arg0 _arg0, _Arg1 _arg1, _Arg2 _arg2, _Arg3 _arg3, _Arg4 _arg4) mutable {
      for (auto listener = event.checked_listeners_.begin();
           listener != event.checked_listeners_.end();) {
        if (auto _observer = (std::get<0>(*listener)).lock()) {
          auto callback = std::get<2>(*listener);
          _observer->send([=]() mutable {
            callback(_arg0, _arg1, _arg2, _arg3, _arg4);
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

}

#endif //ICC_EVENT_HPP
