/**
 * @file api.hpp
 * @author Denis Kotov
 * @date 24 May 2021
 * @brief Contains EXPORT and IMPORT macros
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_PRIVATE_API_HPP
#define ICC_PRIVATE_API_HPP

#if !defined(ICC_LIBRARY)
  #include "api_trigger.hpp"
#endif

#if defined(_MSC_VER)
  // MSVC
  #define ICC_EXPORT __declspec(dllexport)
  #define ICC_IMPORT __declspec(dllimport)
#else
  // GCC, Clang, Apple Clang
  #define ICC_EXPORT __attribute__((visibility("default")))
  #define ICC_IMPORT
#endif

#if defined(ICC_SHARED_LIBRARY)
  #if defined(ICC_LIBRARY)
    #define ICC_PUBLIC ICC_EXPORT
  #else
    #define ICC_PUBLIC ICC_IMPORT
  #endif
#else
  #define ICC_PUBLIC
#endif

#endif //ICC_PRIVATE_API_HPP
