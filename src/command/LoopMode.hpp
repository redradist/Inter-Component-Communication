/**
 * @file LoopMode.hpp
 * @author Denis Kotov
 * @date 23 Mar 2018
 * @brief Contains CommandLoop LoopMode
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef COMMONAPI_CLIENT_LOOPMODE_HPP
#define COMMONAPI_CLIENT_LOOPMODE_HPP

#include <cstdint>
#include <type_traits>

namespace icc {

namespace command {

enum class LoopMode : int32_t {
  /**
   * Default mode (Finite [!Continuous],
   *               SingleCommand [!MultiCommand],
   *               Each [!Transaction])
   */
  Default = 0,
  /**
   * Should be used for setting one time mode
   */
  Continuous = 1,
  /**
   * When set this mode it means that all new added commands will be started
   */
  MultiCommand = 2,
  /**
   * When set this mode it means that commands will behave like a transaction
   */
  Transaction = 4,
};

using LoopModeType = std::underlying_type<LoopMode>::type;

inline
bool operator &(const LoopMode &_left, const LoopMode &_right) {
  return static_cast<bool>(
            static_cast<LoopModeType>(_left) &
            static_cast<LoopModeType>(_right));
}

inline
LoopMode operator |(const LoopMode &_left, const LoopMode &_right) {
  return static_cast<LoopMode>(
            static_cast<LoopModeType>(_left) |
            static_cast<LoopModeType>(_right));
}

}

}

#endif //COMMONAPI_CLIENT_LOOPMODE_HPP
