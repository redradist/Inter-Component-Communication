/**
 * @file Component.hpp
 * @author Denis Kotov
 * @date 10 Jun 2017
 * @brief Contains abstract class for Component
 * @copyright MIT License. Open source: https://github.com/redradist/Transport_Buffers.git
 */

#ifndef ICC_COMPONENT_HPP
#define ICC_COMPONENT_HPP

#include <memory>
#include <thread>
#include <algorithm>
#include <boost/asio/io_service.hpp>
#include "EventLoopProvider.hpp"

class IComponent
  : public IEventLoopProvider {
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
   *
   * @param _eventLoop
   */
  IComponent(boost::asio::io_service * _eventLoop)
      : service_(std::shared_ptr<boost::asio::io_service>(_eventLoop,
  [=](boost::asio::io_service *){
    // NOTE(redra): Nothing need to do. Owner of this pointer is not we
  })),
        worker_(new boost::asio::io_service::work(*service_)) {
  }

  /**
   *
   * @param _eventLoop
   */
  IComponent(std::shared_ptr<boost::asio::io_service> _eventLoop)
    : service_(_eventLoop),
      worker_(new boost::asio::io_service::work(*service_)) {
  }

  /**
   * Used to share event loop of parent object
   * @param _parent
   */
  IComponent(IComponent * _parent)
    : service_(_parent->getEventLoop()),
      worker_(new boost::asio::io_service::work(*service_)),
      parent_(_parent) {
    _parent->setChild(this);
  }

  /**
   * Disable ability to copy Component class
   */
  IComponent(IComponent const &) = delete;
  IComponent & operator=(IComponent const &) = delete;

  /**
   *
   */
  virtual ~IComponent() {
    if (parent_) {
      parent_->removeChild(this);
    } else if (worker_thread_) {
      worker_thread_->join();
    }
  }

 public:
  /**
   *
   */
  void exec() {
    if (owner_of_service_ && !worker_thread_) {
      worker_thread_ = new std::thread([=](){
        service_->run();
      });
    }
  }

  /**
   * Method used to push task for execution
   * @param _task Task that will be executed
   */
  void push(std::function<void(void)> _task) {
    service_->post(_task);
  }

  /**
   * Method return used io_service
   * @return IO Service
   */
  std::shared_ptr<boost::asio::io_service>
  getEventLoop() const override {
    return service_;
  }

 protected:
  /**
   * Called to exit from execution in main loop
   */
  virtual void exit() {
    push([=]{
       worker_.reset();
       for (auto & child : childern_) {
         child->exit();
       }
    });
  }

  /**
   * Method that allow to add _child component to the vector
   * @param _child Component that will be added
   */
  virtual void setChild(IComponent * _child) {
    if (_child) {
      push([=]{
        childern_.push_back(_child);
      });
    }
  }

  /**
   * Method that allow to delete _child component from the vector
   * @param _child Component that will be deleted
   */
  virtual void removeChild(IComponent * _child) {
    if (_child) {
      push([=]{
        auto childIter = std::find(childern_.begin(),
                                   childern_.end(),
                                   _child);
        if (childIter != childern_.end()) {
          childern_.erase(childIter);
        }
      });
    }
  }

 protected:
  bool owner_of_service_ = false;
  std::shared_ptr<boost::asio::io_service> service_;
  std::unique_ptr<boost::asio::io_service::work> worker_;
  std::thread * worker_thread_ = nullptr;

 private:
  IComponent * parent_ = nullptr;
  std::vector<IComponent *> childern_;
};

#endif //ICC_COMPONENT_HPP
