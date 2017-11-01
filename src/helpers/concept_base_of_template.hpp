/**
 * @file concept_base_of_template.hpp
 * @author Denis Kotov
 * @date 3 Jun 2017
 * @brief Contains CommonAPIService wrapper class for creating service
 * @copyright MIT License. Open source: https://github.com/redradist/Transport_Buffers.git
 */

#ifndef COMMONAPI_SERVER_COPCEPT_BASE_OF_TEMPLATE_HPP
#define COMMONAPI_SERVER_COPCEPT_BASE_OF_TEMPLATE_HPP

#include <iostream>
#include <type_traits>

namespace icc {

namespace helpers {

template<template<typename...> class C, typename...Ts>
std::true_type is_base_of_template_impl(const C<Ts...> *);

template<template<typename...> class C>
std::false_type is_base_of_template_impl(...);

template<typename T, template<typename...> class C>
using is_base_of_template = decltype(is_base_of_template_impl<C>(std::declval<T *>()));

}

}

#endif //COMMONAPI_SERVER_COPCEPT_BASE_OF_TEMPLATE_HPP
