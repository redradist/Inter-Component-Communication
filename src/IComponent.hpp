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

#include <memory>
#include <thread>
#include <algorithm>
#include <boost/asio/io_service.hpp>

namespace icc {

class IComponent {
 public:
  /**
   * Default constructor.
   * Only with this constructor object will be owner of service_.
   */
  IComponent()
      : owner_of_service_(true),
        service_(std::make_shared<boost::asio::io_service>()),
        worker_(new boost::asio::io_service::work(*service_)) {
  }

  /**
   * Delegate constructor
   */
  IComponent(std::nullptr_t)
      : IComponent() {
  }

  /**
   * Constructor for initializing within event loop created outside.
   * Owner of this pointer is not we
   * @param _eventLoop Event loop that will be used
   */
  IComponent(boost::asio::io_service *_eventLoop)
      : service_(std::shared_ptr<boost::asio::io_service>(_eventLoop,
                                                          [=](boost::asio::io_service *) {
                                                            // NOTE(redra): Nothing need to do. Owner of this pointer is not we
                                                          })),
        worker_(new boost::asio::io_service::work(*service_)) {
  }

  /**
   * Constructor for initializing within event loop created outside
   * @param _eventLoop Event loop that will be used
   */
  IComponent(std::shared_ptr<boost::asio::io_service> _eventLoop)
      : service_(_eventLoop),
        worker_(new boost::asio::io_service::work(*service_)) {
  }

  /**
   * Used to share event loop of parent object
   * @param _parent Parent compenent that will share event loop
   */
  IComponent(IComponent *_parent)
      : service_(_parent->getEventLoop()),
        worker_(new boost::asio::io_service::work(*service_)),
        parent_(_parent) {
    _parent->addChild(this);
  }

  /**
   * Used to share event loop of parent object
   * @param _parent Parent compenent that will share event loop
   */
  IComponent(std::shared_ptr<IComponent> _parent)
      : service_(_parent->getEventLoop()),
        worker_(new boost::asio::io_service::work(*service_)),
        parent_(_parent.get()) {
    _parent->addChild(this);
  }

  /**
   * Disable ability to copy Component class
   */
  IComponent(IComponent const &) = delete;
  IComponent &operator=(IComponent const &) = delete;

  /**
   * Destructor used for removing children or waiting end of event loop
   */
  virtual ~IComponent() = 0;

 public:
  /**
   * Used to start event loop
   */
  virtual void exec() {
    if (worker_ && owner_of_service_) {
      service_->run();
    }
  }

  /**
   * Called to exit from execution in main loop
   */
  virtual void exit() {
    invoke([=] {
      if (worker_) {
        worker_.reset(nullptr);
        for (auto &child : children_) {
          child->exit();
        }
        if (parent_) {
          parent_->removeChild(this);
        }
      }
    });
  }

  /**
   * Method used to push task for execution
   * @param _task Task that will be executed
   */
  virtual void push(std::function<void(void)> _task) {
    if (worker_) {
      service_->post(_task);
    }
  }

  /**
   * Method used to call task in this thread if
   * current context is io::service itself otherwise
   * push it in queue
   * @param _task Task that will be executed
   */
  virtual void invoke(std::function<void(void)> _task) {
    if (worker_) {
      service_->dispatch(_task);
    }
  }

 protected:
  /**
   * Override this method if you need to track finishing of child classes
   * @param _child Child that was removed
   */
  virtual void onChildExit(IComponent * _child) {
    // NOTE(redra): Default implementation doing nothing
  }

  /**
   * Method return used io_service
   * @return IO Service
   */
  virtual std::shared_ptr<boost::asio::io_service>
  getEventLoop() const {
    return service_;
  }

  /**
   * Method that allow to add _child component to the vector
   * @param _child Component that will be added
   */
  virtual void addChild(IComponent *_child) {
    if (_child) {
      invoke([=] {
        if (worker_) {
          children_.push_back(_child);
        }
      });
    }
  }

  /**
   * Method that allow to delete _child component from the vector
   * @param _child Component that will be deleted
   */
  virtual void removeChild(IComponent *_child) {
    if (_child) {
      invoke([=] {
        if (worker_) {
          auto childIter = std::find(children_.begin(),
                                     children_.end(),
                                     _child);
          if (childIter != children_.end()) {
            children_.erase(childIter);
            onChildExit(_child);
          }
        }
      });
    }
  }

 protected:
  bool owner_of_service_ = false;
  std::shared_ptr<boost::asio::io_service> service_;
  std::unique_ptr<boost::asio::io_service::work> worker_;

 private:
  IComponent *parent_ = nullptr;
  std::vector<IComponent *> children_;
};

/**
 * Destructor used for removing children or waiting end of event loop
 */
inline
IComponent::~IComponent() {
}

}

#endif //ICC_COMPONENT_HPP
