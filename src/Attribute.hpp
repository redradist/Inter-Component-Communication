/**
 * @file Attribute.hpp
 * @author Denis Kotov
 * @date 06 Nov 2017
 * @brief Contains Attribute class.
 * It is an attribute class based on icc::Event class
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef FORECAST_ATTRIBUTE_HPP
#define FORECAST_ATTRIBUTE_HPP

#include "Event.hpp"

namespace icc {

/**
 * Attribute class that is used for storing some value and
 * notify when it is changed
 * @tparam _Field
 */
template<typename _Field>
class Attribute
    : public Event<void(const _Field &)> {
 public:
  /**
   * Method is used to set attribute value
   * @param _field Value to set
   */
  void setAttribute(const _Field &_field) {
    if (!isEqual(_field)) {
      field_ = _field;
      operator()(field_);
    }
  }

  /**
   * Method is used to move attribute value
   * @param _field Value to move
   */
  void setAttribute(_Field && _field) {
    if (!isEqual(_field)) {
      field_ = std::move(_field);
      operator()(field_);
    }
  }

  /**
   * Method is used to get attribute value
   * @return Attribute value
   */
  _Field getAttribute() {
    return field_;
  }

 protected:
  /**
   * Helper method is used to check that fields are equal
   * @param _field
   * @return
   */
  bool isEqual(const _Field &_field) const {
    return field_ == _field;
  }

 private:
  _Field field_;
};

}

#endif //FORECAST_ATTRIBUTE_HPP
