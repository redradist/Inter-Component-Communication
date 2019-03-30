/**
 * @file ICommandHandler.hpp
 * @author Denis Kotov
 * @date 31 Mar 2018
 * @brief
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef COMMANDLOOP_ICOMMANDHANDLER_HPP
#define COMMANDLOOP_ICOMMANDHANDLER_HPP

namespace icc {

namespace command {

class ICommandHandler {
 protected:
  /**
   * Used to start Command
   */
  virtual void processStartCommand() = 0;
  /**
   * Used to resume Command
   */
  virtual void processResumeCommand() = 0;
  /**
   * Used to suspend Command
   */
  virtual void processSuspendCommand() = 0;

  /**
   * Used to stop Command
   */
  virtual void processStopCommand() = 0;
};

}

}

#endif //COMMANDLOOP_ICOMMANDHANDLER_HPP
