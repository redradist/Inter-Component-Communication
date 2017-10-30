//
// Created by redra on 28.10.17.
//

#ifndef ICC_COMMANDDATA_H
#define ICC_COMMANDDATA_H

#include <memory>
#include "ICommandLisener.hpp"
#include "ICommand.hpp"

namespace icc {

namespace command {

struct CommandData {
  std::shared_ptr<ICommand> p_command_;
  CommandResult             result_;
};

}

}

#endif //ICC_COMMANDDATA_H
