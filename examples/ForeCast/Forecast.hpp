//
// Created by redra on 25.06.17.
//

#ifndef ICC_FORECAST_HPP
#define ICC_FORECAST_HPP

#include <functional>
#include "Event.hpp"

class Forecast {
 public:
  virtual void enable() = 0;
  virtual void setIntervalForUpdate(const int &) = 0;

 public:
  icc::Event<void(const double &, int)> temperature_;
  icc::Event<void(const double &,int, int, int, int)> temperature2_;
};

#endif //ICC_FORECAST_HPP
