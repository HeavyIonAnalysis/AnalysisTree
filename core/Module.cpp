/* Copyright (C) 2019-2021 GSI, Universität Tübingen, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Evgeny Kashirin, Ilya Selyuzhenkov */
#include "Module.hpp"
#include "DataHeader.hpp"
#include <iostream>

namespace AnalysisTree {

void CheckCurrentDataHeader() {
  if (!gCurrentDataHeader)
    throw std::runtime_error("No data header defined in the context. Check gCurrentDataHeader or read DataHeader from file first.");
}

bool operator==(const AnalysisTree::Module& that, const AnalysisTree::Module& other) {
  if (&that == &other) {
    return true;
  }
  if ((Container&) that != (Container&) other) {
    return false;
  }
  return that.number_ == other.number_ && that.signal_ == other.signal_;
}

void Module::Print() const noexcept {
  std::cout << "  number = " << number_ << "  signal = " << signal_ << std::endl;
  Container::Print();
}

Floating_t Module::GetX() const {
  CheckCurrentDataHeader();
  return gCurrentDataHeader->GetModulePositions(0).GetChannel(number_-1).GetX(); // TODO idet
}
Floating_t Module::GetY() const {
  CheckCurrentDataHeader();
  return gCurrentDataHeader->GetModulePositions(0).GetChannel(number_-1).GetY(); // TODO idet
}
Floating_t Module::GetZ() const {
  CheckCurrentDataHeader();
  return gCurrentDataHeader->GetModulePositions(0).GetChannel(number_-1).GetZ(); // TODO idet
}
Floating_t Module::GetPhi() const {
  CheckCurrentDataHeader();
  return gCurrentDataHeader->GetModulePositions(0).GetChannel(number_-1).GetPhi(); // TODO idet
}

void ModulePosition::Print() const {
  std::cout << "  x = " << x_ << "  y = " << y_ << "  z = " << z_ << "  phi = " << GetPhi() << std::endl;
}
}// namespace AnalysisTree