/**
 * @file IComponent.hpp
 * @author Denis Kotov
 * @date 10 Jun 2017
 * @brief Contains abstract class for Component.
 * It is thread safe version of Component class
 * Can be used to create different kind of architectures
 * without of worrying of thread safety
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_COMPONENT_HPP
#define ICC_COMPONENT_HPP

#include <utility>
#include <memory>
#include <thread>
#include <vector>
#include <algorithm>
#include "Context.hpp"
#include <icc/_private/containers/ThreadSafeQueue.hpp>

namespace icc {

class Component {
 private:
  /**
   * Default constructor.
   * Only with this constructor object will be owner of service_.
   */
  Component()
    : context_(ContextBuilder::createContext<ThreadSafeQueueAction>())
    , channel_(context_->createChannel()) {
  }

 public:
  /**
   * Delegate constructor.
   * Use only in case if you want to be an OWNER of event loop !!
   */
  explicit Component(std::nullptr_t)
      : Component() {
  }

  /**
   * Constructor for initializing within event loop created outside.
   * Owner of this pointer is not we
   * @param _service Service for creating event loop that will be used
   */
  template <typename TService,
            typename = typename std::enable_if<!std::is_base_of<Component, TService>::value>::type,
            typename = typename std::enable_if<!std::is_base_of<IContext, TService>::value>::type,
            typename = typename std::enable_if<!std::is_base_of<IContext::IChannel, TService>::value>::type>
  explicit Component(TService *_service)
    : channel_(ContextBuilder::createContext(_service)->createChannel()) {
  }

  /**
   * Constructor for initializing within event loop created outside.
   * Owner of this pointer is not we
   * @param _eventLoop Service for creating event loop that will be used
   */
  template <typename TService,
            typename = typename std::enable_if<!std::is_base_of<Component, TService>::value>::type,
            typename = typename std::enable_if<!std::is_base_of<IContext, TService>::value>::type,
            typename = typename std::enable_if<!std::is_base_of<IContext::IChannel, TService>::value>::type>
  explicit Component(std::shared_ptr<TService> _service)
    : channel_(ContextBuilder::createContext(_service)->createChannel()) {
    static_assert(!std::is_base_of<Component, TService>::value,
                  "_listener is not derived from Component");
  }

  /**
   *
   * @param _context
   */
  explicit Component(std::shared_ptr<IContext> _context)
    : channel_(std::move(_context->createChannel())) {
  }

  /**
   * Constructor for initializing within event loop created outside
   * @param _eventLoop Event loop that will be used
   */
  explicit Component(std::unique_ptr<IContext::IChannel> _channel)
    : channel_(std::move(_channel)) {
  }

  /**
   * Used to share event loop of parent object
   * @param _parent Parent compenent that will share event loop
   */
  explicit Component(Component *_parent)
    : channel_(_parent->getContext().createChannel())
    , parent_(_parent) {
    _parent->addChild(this);
  }

  /**
   * Used to share event loop of parent object
   * @param _parent Parent compenent that will share event loop
   */
  explicit Component(std::shared_ptr<Component> _parent)
    : channel_(_parent->getContext().createChannel())
    , parent_(_parent.get()) {
    _parent->addChild(this);
  }

  /**
   * Disable ability to copy Component class
   */
  Component(Component const &) = delete;
  Component &operator=(Component const &) = delete;

  /**
   * Destructor used for removing children or waiting end of event loop
   */
  virtual ~Component() = 0;

 public:
  /**
   * Used to start event loop
   */
  virtual void exec() {
    if (context_) {
      context_->run();
    }
  }

  virtual void stop() {
    channel_.reset();
    if (context_) {
      context_->stop();
    }
  }

  /**
   * Called to exit from execution in main loop
   */
  virtual void exit() {
    invoke([=] {
      stop();
      for (auto &child : children_) {
        child->exit();
      }
      if (parent_) {
        parent_->removeChild(this);
      }
    });
  }

  /**
   * Method used to push task for execution
   * @param _task Task that will be executed
   */
  virtual void push(std::function<void(void)> _task) {
    if (channel_) {
      channel_->push(std::move(_task));
    }
  }

  /**
   * Method used to call task in this thread if
   * current context is io::service itself otherwise
   * push it in queue
   * @param _task Task that will be executed
   */
  virtual void invoke(std::function<void(void)> _task) {
    if (channel_) {
      channel_->invoke(std::move(_task));
    }
  }

 protected:
  /**
   * Override this method if you need to track finishing of child classes
   * @param _child Child that was removed
   */
  virtual void onChildExit(Component * _child) {
    // NOTE(redra): Default implementation doing nothing
  }

  /**
   * Method return used io_service
   * @return IO Service
   */
#if __cpp_lib_optional >= 201606L
  [[nodiscard]]
#endif
  IContext & getContext() const {
    return channel_->getContext();
  }

  /**
   * Method that allow to add _child component to the vector
   * @param _child Component that will be added
   */
  virtual void addChild(Component *_child) {
    if (_child) {
      invoke([=] {
        children_.push_back(_child);
      });
    }
  }

  /**
   * Method that allow to delete _child component from the vector
   * @param _child Component that will be deleted
   */
  virtual void removeChild(Component *_child) {
    if (_child) {
      invoke([=] {
        auto childIter = std::find(children_.begin(),
                                   children_.end(),
                                   _child);
        if (childIter != children_.end()) {
          children_.erase(childIter);
          onChildExit(_child);
        }
      });
    }
  }

 private:
  std::shared_ptr<ContextBase> context_;
  std::unique_ptr<IContext::IChannel> channel_;
  Component *parent_ = nullptr;
  std::vector<Component *> children_;
};

/**
 * Destructor used for removing children or waiting end of event loop
 */
inline
Component::~Component() {
}

}

#endif //ICC_COMPONENT_HPP
