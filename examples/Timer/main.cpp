//
// Created by redra on 05.07.17.
//

#include <iostream>
#include "IComponent.hpp"
#include "Event.hpp"
#include "Timer.hpp"

class Componet
    : public IComponent,
      public ITimerLisener {
 public:
  using IComponent::IComponent;

  void processEvent(const TimerEvents & _event) override {
    if (TimerEvents::STARTED == _event) {
      std::cout << "Componet: Timer is started" << std::endl;
    } else if (TimerEvents::EXPIRED == _event) {
      std::cout << "Componet: Timer is expired" << std::endl;
    }
  }

  void Callback(const int &, double) {
    std::cout << "Componet: Callback was called" << std::endl;
  }
};

int main() {
  Componet parent;
  Componet client(&parent);
  Componet subClient(&client);

  Timer timer2_(&parent);
  timer2_.setInterval(boost::posix_time::seconds(2));
  timer2_.setNumberOfRepetition(5);
  timer2_.addListener(&parent);
  timer2_.connect(&Componet::processEvent, &client);
  timer2_.addListener(&subClient);
  timer2_.start();

  // Start event loop
  parent.exec();
  return 0;
}
