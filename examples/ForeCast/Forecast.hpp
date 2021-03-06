//
// Created by redra on 25.06.17.
//

#ifndef ICC_FORECAST_HPP
#define ICC_FORECAST_HPP

#include <functional>
#include "icc/Event.hpp"
#include "icc/Attribute.hpp"

class Forecast {
 public:
  virtual void setIntervalForUpdate(int &) = 0;

 public:
  icc::Event<void(const double &)> temperature_;
  icc::Attribute<double> temperature2_;
};

#endif //ICC_FORECAST_HPP
