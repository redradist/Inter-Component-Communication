/**
 * @file Builder.hpp
 * @author Denis Kotov
 * @date 11 Jan 2018
 * @brief Implementation of builders for Command-s and CommandLoop-s
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef COMMAND_BUILDER_HPP
#define COMMAND_BUILDER_HPP

#include <memory>
#include <type_traits>
#include "Command.hpp"
#include "CommandLoop.hpp"

namespace icc {

namespace command {

class Builder {
 public:
  template<typename _Command = Command, typename ... _Args>
  static std::shared_ptr<_Command> buildCommand(_Args &&... _args) {
    static_assert(std::is_base_of<ICommand, _Command>::value,
                  "_Command is not an ICommand class");
    return std::shared_ptr<_Command>(new _Command(std::forward<_Args>(_args)...));
  }

  template<typename _CommandLoop = CommandLoop, typename ... _Args>
  static std::shared_ptr<_CommandLoop> buildCommandLoop(_Args &&... _args) {
    static_assert(std::is_base_of<ICommandLoop, _CommandLoop>::value,
                  "_CommandLoop is not an ICommandLoop class");
    return std::shared_ptr<_CommandLoop>(new _CommandLoop(std::forward<_Args>(_args)...));
  }
};

}

}

#endif //COMMAND_BUILDER_HPP
