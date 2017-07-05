#include <iostream>
#include "src/IComponent.hpp"
#include "src/Event.hpp"
#include "src/Timer.hpp"
#include "service/IService.hpp"
#include "service/IClient.hpp"
#include "InterfaceForinterface.hpp"


class NewService
    : public virtual IComponent,
      public IService<InterfaceForInterface> {
 public:

  NewService(IComponent * _parent)
      : IComponent(_parent),
        IService<InterfaceForInterface>("NewService") {
  }

  ~NewService() {
    std::cout << "~NewService" << std::endl;
  }

  void addVersion(std::function<void(std::string)> _reply) override {
    std::cout << "addVersion from NewService" << std::endl;
    event_(5);
    _reply("Denis");
  }

  virtual void addVersion2(std::string) override {
    std::cout << "addVersion2 from NewService" << std::endl;
  }

};

class CompositeObject
: public IService<InterfaceForInterface>,
  public IClient<InterfaceForInterface>,
  public ITimerLisener {
 public:

  CompositeObject(IComponent * _parent)
      : IComponent(_parent),
        IService<InterfaceForInterface>("NewService"),
        IClient<InterfaceForInterface>("NewService") {

  }

  void processEvent(const TimerEvents & _event) override {
    if (TimerEvents::STARTED == _event) {
      std::cout << "NewClient: Timer is started" << std::endl;
    } else if (TimerEvents::EXPIRED == _event) {
      std::cout << "NewClient: Timer is expired" << std::endl;
    }
  }

  void addVersion(std::function<void(std::string)> _reply) override {
    std::cout << "addVersion from NewService" << std::endl;
    event_(5);
    _reply("Denis");
  }

  virtual void addVersion2(std::string) override {
    std::cout << "addVersion2 from NewService" << std::endl;
  }

  void callback2(const int & i, double h) {
    std::cout << "NewClient: callback2 = " << i << std::endl;
  }

  void callback(const int & n) {
    std::cout << "NewClient: callback = " << n << std::endl;
  }

  void connected(InterfaceForInterface*) override {
    std::cout << "connected is called" << std::endl;
    std::function<void(std::string)> kjh = [](std::string str){
      std::cout << "Hello " << str << std::endl;
    };
    call(&InterfaceForInterface::addVersion, kjh);
    std::string sda;
    //call(&InterfaceForInterface::addVersion2, sda);
  }

  void disconnected(InterfaceForInterface*) override {
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
      std::cout << "Componet: Timer is started" << std::endl;
    } else if (TimerEvents::EXPIRED == _event) {
      std::cout << "Componet: Timer is expired" << std::endl;
    }
  }

  void Callback(const int & i, double k) {
    std::cout << "Componet: Callback was called " << i << std::endl;
  }
};

void dsfdfsdf(const boost::system::error_code& ec) {

}

int main() {

  std::cout << "Hello, World!" << std::endl;
  boost::asio::io_service service_;
  Componet com;
  std::shared_ptr<Componet> com12 = std::shared_ptr<Componet>(new Componet());
  Componet com1(&com);
  Componet com2(&com1);

  std::shared_ptr<CompositeObject> composite = std::make_shared<CompositeObject>(&com);
  composite->registerService();
  //std::shared_ptr<NewClient> client = std::make_shared<NewClient>(&com);
  composite->subscribe(&InterfaceForInterface::event_, &CompositeObject::callback);
  composite->call(&InterfaceForInterface::addVersion, std::function<void(std::string)>([](std::string _name){
    std::cout << "Response to " << _name << std::endl;
  }));
  //client->subscribe(&InterfaceForInterface::event_, &NewClient::callback);
//  composite->call(&InterfaceForInterface::addVersion, std::function<void(std::string)>([](std::string _name){
//    std::cout << "Response to " << _name << std::endl;
//  }));
  composite->call(&InterfaceForInterface::addVersion2, std::string());

//  auto are1 = std::thread([=]() {
//    service->exec();
//  });
//
//  auto are2 = std::thread([=]() {
//    client->exec();
//  });

  Timer timer2_(&com);
  timer2_.setInterval(boost::posix_time::seconds(2));
  timer2_.enableContinuous();
  timer2_.disableContinuous();
  timer2_.setNumberOfRepetition(5);
  timer2_.connect(&Componet::processEvent, &com2);
  timer2_.addListener(&com);
  timer2_.addListener(com12);
  timer2_.addListener(composite);
  timer2_.removeListener(com12);

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
  //sf.call(&InterfaceForInterface::addVersion);
  com.exec();
  service_.run();
  are.join();
  return 0;
}
