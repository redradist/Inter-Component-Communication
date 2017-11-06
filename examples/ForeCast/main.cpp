//
// Created by redra on 05.07.17.
//

#include <iostream>
#include <IComponent.hpp>
#include <Event.hpp>
#include <Timer.hpp>
#include <service/IService.hpp>
#include <service/IClient.hpp>
#include "Forecast.hpp"

class WeatherStation
    : public icc::service::IService<Forecast>,
      public icc::ITimerListener {
 public:
  WeatherStation(boost::asio::io_service & _io_service)
      : icc::IComponent(&_io_service),
        icc::service::IService<Forecast>("WeatherStation"),
        timer_(service_) {
    std::cout << "WeatherStation" << std::endl;
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
    timer_.setInterval(boost::posix_time::seconds(_seconds));
    timer_.setNumberOfRepetition(icc::Timer::Infinite);
    timer_.addListener(this);
    timer_.start();

    // Experimentation
    temperature2_ = 2;
    double temperature2 = temperature2_;
  }

 protected:
  /**
   * Service with timer should override exit
   * method from parent component for stopping timer
   * and releasing io::service
   */
  void exit() override {
    icc::service::IService<Forecast>::exit();
    timer_.stop();
  }

 private:
  icc::Timer timer_;
};

class TestObserver
    : public icc::IComponent {
 public:
  using icc::IComponent::IComponent;
 public:
  void onTemperature(const double & _temperature) {
    std::cout << "TestObserver::Temperature is " << _temperature << std::endl;
  }
};

class WeatherObserver
    : public icc::service::IClient<Forecast> {
 public:
  WeatherObserver(boost::asio::io_service & _io_service)
      : icc::IComponent(&_io_service),
        icc::service::IClient<Forecast>("WeatherStation") {
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
    subscribe(&Forecast::temperature_, p_test_, &TestObserver::onTemperature);
    // NOTE(redra): The following line added only for testing purposes
    //unsubscribe(&Forecast::temperature_, &WeatherObserver::onTemperature);
    //unsubscribe(&Forecast::temperature_, p_test_, &TestObserver::onTemperature);
  }

  void disconnected(Forecast*) override {
    std::cout << "disconnected is called" << std::endl;
  }
};

int main() {
  boost::asio::io_service service_;
  std::shared_ptr<WeatherObserver> observer = std::make_shared<WeatherObserver>(service_);
  std::shared_ptr<WeatherStation> station = std::make_shared<WeatherStation>(service_);
  // Start event loop
  service_.run();
  return 0;
}
