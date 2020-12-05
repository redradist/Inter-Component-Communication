//
// Created by redra on 05.07.17.
//

#include <iostream>
#include <icc/Component.hpp>
#include <icc/Event.hpp>
#include <icc/Timer.hpp>
#include <icc/localbus/IService.hpp>
#include <icc/localbus/IClient.hpp>
#include "Forecast.hpp"

class WeatherStation
    : public icc::localbus::IService<Forecast>,
      public icc::ITimerListener {
 public:
  WeatherStation(std::shared_ptr<icc::IContext> _io_service)
      : icc::Component(_io_service),
        icc::localbus::IService<Forecast>("WeatherStation"),
        timer_() {
    std::cout << "WeatherStation" << std::endl;
    // Experimentation
    temperature2_ = 22;
  }

  ~WeatherStation() {
    std::cout << "~WeatherStation" << std::endl;
  }

  void processEvent(const icc::TimerEvents & _event) override {
    if (icc::TimerEvents::EXPIRED == _event) {
      std::cout << "processEvent icc::TimerEvents::EXPIRED" << std::endl;
      temperature_(28.3);
    }
  }

  void setIntervalForUpdate(int & _seconds) override {
    std::cout << "setIntervalForUpdate: seconds = " << _seconds << std::endl;
    timer_.setInterval(std::chrono::seconds(_seconds));
    timer_.setNumberOfRepetition(icc::Timer::Infinite);
    timer_.addListener(this);
    timer_.start();
  }

 protected:
  /**
   * Service with timer should override exit
   * method from parent component for stopping timer
   * and releasing io::service
   */
  void exit() override {
    icc::localbus::IService<Forecast>::exit();
    timer_.stop();
  }

 private:
  icc::Timer timer_;
};

class TestObserver
    : public icc::Component {
 public:
  using icc::Component::Component;
 public:
  void onTemperature(const double & _temperature) {
    std::cout << "TestObserver::Temperature is " << _temperature << std::endl;
  }
};

class WeatherObserver
    : public icc::localbus::IClient<Forecast> {
 public:
  WeatherObserver(std::shared_ptr<icc::IContext> _io_service)
      : icc::Component(_io_service),
        icc::localbus::IClient<Forecast>("WeatherStation") {
    std::cout << "WeatherObserver" << std::endl;
  }

  ~WeatherObserver() {
    std::cout << "~WeatherObserver" << std::endl;
  }
  void onTemperature(const double & _temperature) {
    std::cout << "WeatherObserver::Temperature is " << _temperature << std::endl;
  }

 protected:
  std::shared_ptr<TestObserver> p_test_ = std::make_shared<TestObserver>(this);

  void connected(Forecast*) override {
    std::cout << "connected is called" << std::endl;
    int i = 7;
    call(&Forecast::setIntervalForUpdate, i);
    subscribe(&Forecast::temperature_, &WeatherObserver::onTemperature);
    subscribe(&Forecast::temperature2_, &WeatherObserver::onTemperature);
    subscribe(&Forecast::temperature_, p_test_, &TestObserver::onTemperature);
    subscribe(&Forecast::temperature2_, p_test_, &TestObserver::onTemperature);
    // NOTE(redra): The following line added only for testing purposes
    //  unsubscribe(&Forecast::temperature_, &WeatherObserver::onTemperature);
    //  unsubscribe(&Forecast::temperature2_, &WeatherObserver::onTemperature);
    //  unsubscribe(&Forecast::temperature_, p_test_, &TestObserver::onTemperature);
    //  unsubscribe(&Forecast::temperature2_, p_test_, &TestObserver::onTemperature);
  }

  void disconnected(Forecast*) override {
    std::cout << "disconnected is called" << std::endl;
  }
};

int main() {
  auto defaultContext = icc::ContextBuilder::createContext<icc::ThreadSafeQueueAction>();
  std::shared_ptr<WeatherObserver> observer = std::make_shared<WeatherObserver>(defaultContext);
  std::shared_ptr<WeatherStation> station = std::make_shared<WeatherStation>(defaultContext);
  // Start event loop
  defaultContext->run();
  return 0;
}
