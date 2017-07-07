//
// Created by redra on 08.07.17.
//

#ifndef ICC_ICOMMAND_HPP
#define ICC_ICOMMAND_HPP

#include <IComponent.hpp>

namespace icc {

namespace command {

enum class CommandResult {
  SUCCESFULLY,
  FAILED,
  STOPED,
};

class ICommand
    : public IComponent {
 public:
  virtual void start();
  virtual void stop();

 protected:
  void finished(CommandResult);

};

}

}

#endif //ICC_ICOMMAND_HPP
