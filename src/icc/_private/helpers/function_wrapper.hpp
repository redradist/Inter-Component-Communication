//
// Created by redra on 04.06.19.
//

#ifndef ICC_FUNCTION_WRAPPER_HPP
#define ICC_FUNCTION_WRAPPER_HPP

#include <functional>
#include "memory_helpers.hpp"

namespace icc {

namespace _private {

namespace helpers {

template <typename T>
class function_wrapper;

template <typename TRes, typename ... TArgs>
class function_wrapper<TRes(TArgs...)> {
 public:
  function_wrapper(TRes(*func)(TArgs...)) {
    function_ptr_ = reinterpret_cast<void *>(func);
    callback_ = func;
  }

  template <typename TObject>
  function_wrapper(TRes(TObject::*_method)(TArgs...),
                   TObject *_object) {
    object_ptr_ = reinterpret_cast<void *>(_object);
    method_ptr_ = icc::helpers::void_cast(_method);
    callback_ = [_object, _method] (TArgs ... _args) {
      return (_object->*_method)(_args...);
    };
  }

  TRes operator()(TArgs ... _args) const {
    return callback_(_args...);
  }

  bool operator ==(const function_wrapper & wrapper) const {
    return function_ptr_ == wrapper.function_ptr_ &&
           object_ptr_ == wrapper.object_ptr_ &&
           method_ptr_ == wrapper.method_ptr_;
  }

  bool operator !=(const function_wrapper & wrapper) const {
    return !operator==(wrapper);
  }

 private:
  std::function<TRes(TArgs...)> callback_;
  void *function_ptr_ = nullptr;
  void *object_ptr_ = nullptr;
  void *method_ptr_ = nullptr;
};

template <typename ... TArgs>
class function_wrapper<void(TArgs...)> {
 public:
  function_wrapper(void(*func)(TArgs...)) {
    function_ptr_ = reinterpret_cast<void *>(func);
    callback_ = func;
  }

  template<typename TObject>
  function_wrapper(void(TObject::*_method)(TArgs...),
                   TObject *_object) {
    object_ptr_ = reinterpret_cast<void *>(_object);
    method_ptr_ = icc::helpers::void_cast(_method);
    callback_ = [_object, _method] (TArgs ... _args) {
      (_object->*_method)(_args...);
    };
  }

  void operator()(TArgs ... _args) const {
    callback_(_args...);
  }

  bool operator ==(const function_wrapper & wrapper) const {
    return function_ptr_ == wrapper.function_ptr_ &&
           object_ptr_ == wrapper.object_ptr_ &&
           method_ptr_ == wrapper.method_ptr_;
  }

  bool operator !=(const function_wrapper & wrapper) const {
    return !operator==(wrapper);
  }

 private:
  std::function<void(TArgs...)> callback_;
  void *function_ptr_ = nullptr;
  void *object_ptr_ = nullptr;
  void *method_ptr_ = nullptr;
};

}

}

}

#endif //ICC_FUNCTION_WRAPPER_HPP
