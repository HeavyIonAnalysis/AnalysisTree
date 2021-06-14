/* Copyright (C) 2019-2021 GSI, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Eugeny Kashirin, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_INFRA_VALUEHOLDER_HPP_
#define ANALYSISTREE_INFRA_VALUEHOLDER_HPP_

#include <type_traits>

#include "EventHeader.hpp"
#include "Detector.hpp"
#include "Field.hpp"

namespace AnalysisTree {

class Branch;
class BranchChannel;

class ValueHolder {
 public:
  using ChannelPointer = ANALYSISTREE_UTILS_VARIANT<Track*, Particle*, Module*, Hit*, EventHeader*>;

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
  inline ValueHolder& operator=(bool new_val) {
    SetVal(bool(new_val));
    return *this;
  }
  ValueHolder& operator=(const ValueHolder& other);

 private:
  friend Branch;
  friend BranchChannel;

//  template<class T>
  ValueHolder(const AnalysisTree::Field& v, ChannelPointer data_ptr)
      : v(v), data_ptr(data_ptr) {}

  const AnalysisTree::Field& v;
  ChannelPointer data_ptr;
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_INFRA_VALUEHOLDER_HPP_
