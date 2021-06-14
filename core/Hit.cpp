/* Copyright (C) 2019-2021 GSI, Universität Tübingen, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Eugeny Kashirin, Ilya Selyuzhenkov */
#include "Hit.hpp"

#include <iostream>

namespace AnalysisTree {

void Hit::Print() const noexcept {
  std::cout << "  x = " << x_ << "  y = " << y_ << "  z = " << z_ << "  phi = " << GetPhi() << "  signal = " << signal_ << std::endl;
  Container::Print();
}

bool operator==(const Hit& that, const Hit& other) {
  if (&that == &other) {
    return true;
  }

  if ((Container&) that != (Container&) other) {
    return false;
  }

  return that.x_ == other.x_ && that.y_ == other.y_ && that.z_ == other.z_ && that.signal_ == other.signal_;
}

Floating_t Hit::GetPhi() const { return atan2(y_, x_); }

}// namespace AnalysisTree