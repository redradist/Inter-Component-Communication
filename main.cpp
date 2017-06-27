#include <iostream>
#include "src/IComponent.hpp"
#include "src/Event.hpp"
#include "src/Timer.hpp"
#include "src/service/IService.hpp"
#include "src/service/IClient.hpp"
#include "InterfaceForinterface.hpp"

class NewService
    : public IService<InterfaceForInterface> {
 public:

  NewService()
      : IService<InterfaceForInterface>("NewService") {
  }

  void addVersion() override {
    std::cout << "addVersion from NewService" << std::endl;
  }

};

class NewClient
: public IClient<InterfaceForInterface> {
 public:

  NewClient()
      : IClient<InterfaceForInterface>("NewService") {

  }

  void connected(InterfaceForInterface&) override {
    std::cout << "connected is called" << std::endl;
    call(&InterfaceForInterface::addVersion);
  }

  void disconnected(InterfaceForInterface&) override {
    std::cout << "disconnected is called" << std::endl;
  }
//
//  ASDfdgsdgfsdgs * operator->() {
//    std::cout << "Wrapped call" << std::endl;
//    return this;
//  }
};

class Componet
    : public IComponent,
      public ITimerLisener {
 public:
  using IComponent::IComponent;

  Event<void(const int &, double)> event_;

  //Timer timer_;

  void processEvent(const TimerEvents & _event) override {
    if (TimerEvents::STARTED == _event) {
      std::cout << "Timer is started" << std::endl;
    } else if (TimerEvents::EXPIRED == _event) {
      std::cout << "Timer is expired" << std::endl;
    }
  }

  void Callback(const int & i, double k) {
    std::cout << "Callback was called " << i << std::endl;
    exit();
  }
};

void dsfdfsdf(const boost::system::error_code& ec) {

}

int main() {
  NewClient sf;

  auto are2 = std::thread([&]() {
    sf.exec();
  });
  sf.call(&InterfaceForInterface::addVersion);

  std::cout << "Hello, World!" << std::endl;
  boost::asio::io_service service_;
  Componet com(&service_);
  std::shared_ptr<Componet> com12 = std::shared_ptr<Componet>(new Componet());
  Componet com1(&com);
  Componet com2(&com1);

  Timer timer2_(&com);
  timer2_.setInterval(boost::posix_time::seconds(2));
  timer2_.enableContinuous();
  timer2_.disableContinuous();
  timer2_.setNumberOfRepetition(5);
  timer2_.connect(&Componet::processEvent, &com2);
  timer2_.addListener(&com);
  timer2_.addListener(com12);

  //timer2_.addListener(&sf);

  timer2_.start();
  auto are = std::thread([=]() {
    com12->exec();
  });
  std::function<void(const int &, double)> asdas =
      com1.event_;
  {
    std::shared_ptr<Componet> com3 = std::shared_ptr<Componet>(new Componet());
    com1.event_.connect(&Componet::Callback, com3);
  }
  com1.event_.connect(&Componet::Callback, &com2);
  {
    std::shared_ptr<Componet> com3 = std::shared_ptr<Componet>(new Componet());
    com1.event_.connect(&Componet::Callback, com3);
  }
  com1.event_.connect(&Componet::Callback, com12);
  {
    std::shared_ptr<Componet> com3 = std::shared_ptr<Componet>(new Componet());
    com1.event_.connect(&Componet::Callback, com3);
  }
  std::cout << "Before callback" << std::endl;
  const std::function<void(const int &, double)> event3 = com1.event_;
  event3(1, 6);
  std::cout << "After callback" << std::endl;
  sf.call(&InterfaceForInterface::addVersion);
  com.exec();
  std::thread are1;
  NewService service;

  {
    are1 = std::thread([&]() {
      service.exec();
    });
  }

  service_.run();
  are.join();
  return 0;
}