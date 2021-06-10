//
// Created by eugene on 13/03/2021.
//

#ifndef ATTASKSKELETON_ATI2_VALUEHOLDER_HPP_
#define ATTASKSKELETON_ATI2_VALUEHOLDER_HPP_


#include "ATI2_fwd.hpp"
#include "Field.hpp"
#include <type_traits>

namespace ATI2 {

class ValueHolder {
 public:
  float GetVal() const;
  int GetInt() const;
  bool GetBool() const;
  void SetVal(float val) const;
  void SetVal(int int_val) const;
  void SetVal(bool bool_val) const;

  operator float() const;

  template<typename ValueType>
  typename std::enable_if<std::is_floating_point<ValueType>::value, ValueHolder&>::type
  operator=(ValueType new_val) {
    SetVal(float(new_val));
    return *this;
  }
  template<typename ValueType>
  typename std::enable_if<std::is_integral<ValueType>::value, ValueHolder&>::type
  operator=(ValueType new_val) {
    SetVal(int(new_val));
    return *this;
  }
  inline
  ValueHolder &operator=(bool new_val) {
    SetVal(bool(new_val));
    return *this;
  }
  ValueHolder &operator=(const ValueHolder &other);

 private:
  friend Branch;
  friend BranchChannel;

  ValueHolder(const Field &v, void *data_ptr)
      : v(v), data_ptr(data_ptr) {}

  const Field &v;
  void *data_ptr;
};

}
#endif //ATTASKSKELETON_ATI2_VALUEHOLDER_HPP_
