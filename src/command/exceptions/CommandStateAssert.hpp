/**
 * @file CommandStateAssert.hpp
 * @author Denis Kotov
 * @date 24 Mar 2018
 * @brief Contains exception for Command State Assert for ICC library
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef COMMANDLOOP_COMMANDLOOPASSERT_HPP
#define COMMANDLOOP_COMMANDLOOPASSERT_HPP

#include <exceptions/ICCException.hpp>
#include <string>

namespace icc {

namespace command {

class ICommand;

class CommandStateAssert : public icc::ICCException {
 public:
  CommandStateAssert(std::shared_ptr<ICommand> _command,
                     const char * _reason) {
    command_ = _command;
    reason_ = _reason;
  }

  CommandStateAssert(std::shared_ptr<ICommand> _command,
                     const std::string _reason) {
    command_ = _command;
    reason_ = _reason;
  }

  std::shared_ptr<ICommand> getCommand() const  {
    return command_;
  }

  virtual const char *what() const noexcept override {
    return reason_.c_str();
  }

 private:
  std::shared_ptr<ICommand> command_;
  std::string reason_;
};

}

}

#endif //COMMANDLOOP_COMMANDLOOPASSERT_HPP
