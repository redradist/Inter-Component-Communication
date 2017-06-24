#include <iostream>
#include "src/IComponent.hpp"
#include "src/Event.hpp"
#include "src/Timer.hpp"

class Componet
    : public IComponent,
      public ITimerLisener {
 public:
  using IComponent::IComponent;

  Event<void(const int &, double)> event_;

  //Timer timer_;

  void processTimerEvent(const TimerEvents & _event) override {
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
  std::cout << "Hello, World!" << std::endl;
  boost::asio::io_service service_;
  boost::asio::deadline_timer timer_(service_);
  timer_.expires_from_now(boost::posix_time::seconds(2));
  timer_.async_wait(dsfdfsdf);
  Componet com;
  std::shared_ptr<Componet> com12 = std::shared_ptr<Componet>(new Componet());
  Componet com1(&com);
  Componet com2(&com1);

  Timer timer2_(&com);
  timer2_.setInterval(boost::posix_time::seconds(2));
  timer2_.connect(&Componet::processTimerEvent, &com2);
  timer2_.disconnect(&Componet::processTimerEvent, &com2);
  timer2_.addListener(&com);
  timer2_.removeListener(&com);
  timer2_.addListener(com12);
  timer2_.removeListener(com12);
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
  const std::function<void(const int &, double)> event3{com1.event_};
  //event3(1, 6);
  std::cout << "After callback" << std::endl;
  com.exec();
  are.join();
  return 0;
}