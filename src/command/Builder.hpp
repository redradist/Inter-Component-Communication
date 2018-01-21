/**
 * @file Builder.hpp
 * @author Denis Kotov
 * @date 11 Jan 2018
 * @brief Default Command class which just stub virtual methods
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef COMMANDLOOP_BUILDER_HPP
#define COMMANDLOOP_BUILDER_HPP

#include <memory>
#include <type_traits>
#include "ICommand.hpp"
#include "ICommandLoop.hpp"
#include "Command.hpp"
#include "CommandLoop.hpp"

namespace icc {

namespace command {

class Builder {
 public:
  template<typename _Command = Command, typename ... _Args>
  static std::shared_ptr<ICommand> buildCommand(_Args &&... _args) {
    static_assert(std::is_base_of<ICommand, _Command>::value,
                  "_Command is not an ICommand class");
    return std::shared_ptr<ICommand>(new _Command(std::forward<_Args>(_args)...));
  }

  template<typename _CommandLoop = CommandLoop, typename ... _Args>
  static std::shared_ptr<ICommandLoop> buildCommandLoop(_Args &&... _args) {
    static_assert(std::is_base_of<ICommandLoop, _CommandLoop>::value,
                  "_CommandLoop is not an ICommandLoop class");
    return std::shared_ptr<ICommandLoop>(new _CommandLoop(std::forward<_Args>(_args)...));
  }
};

}

}

#endif //COMMANDLOOP_BUILDER_HPP
