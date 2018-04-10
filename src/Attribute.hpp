/**
 * @file Attribute.hpp
 * @author Denis Kotov
 * @date 06 Nov 2017
 * @brief Contains Attribute class.
 * It is an attribute class based on icc::Event class
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_ATTRIBUTE_HPP
#define ICC_ATTRIBUTE_HPP

#include "Event.hpp"

namespace icc {

/**
 * Attribute class that is used for storing some value and
 * notify when it is changed
 * @tparam _Field
 */
template<typename _FieldType,
         typename _Field = typename std::remove_reference<
                             typename std::remove_cv<_FieldType>::type>::type>
class Attribute
    : public Event<void(const _Field &)> {
  static_assert(std::is_copy_constructible<_Field>::value,
                "_Field is not copy constructable !!");
 public:
  /**
   * Method is used to set attribute value
   * @param _field Value to set
   */
  void setValue(const _Field &_field) {
    if (!isEqual(_field)) {
      field_ = _field;
      this->operator()(field_);
    }
  }

  /**
   * Method is used to move attribute value
   * @param _field Value to move
   */
  void setValue(_Field && _field) {
    if (!isEqual(_field)) {
      field_ = std::move(_field);
      this->operator()(field_);
    }
  }

  /**
   * Method is used to get attribute value
   * @return Attribute value
   */
  _Field getValue() {
    return field_;
  }

  /**
   * Helper method is used for simplification of set attribute
   * @param _field Value of attribute to set
   * @return
   */
  Attribute & operator=(const _Field &_field) {
    setValue(_field);
    return *this;
  }

  /**
   * Helper method is used for simplification of
   * move new value into attribute
   * @param _field Value of attribute to set
   * @return
   */
  Attribute & operator=(_Field && _field) {
    setValue(std::move(_field));
    return *this;
  }

  /**
   * Cast Attribute to underlying type
   * @return
   */
  operator _Field() const {
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

#endif //ICC_ATTRIBUTE_HPP
