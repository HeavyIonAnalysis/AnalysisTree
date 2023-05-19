/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "DataHeader.hpp"

#include <iostream>

namespace AnalysisTree {

void DataHeader::Print(Option_t*) const {
  std::cout << "AnalysisTree::DataHeader::Print()" << std::endl;
  std::cout << " Collision system is " << system_ << std::endl;
  std::cout << " at beam momentum " << beam_mom_;
  std::cout << " (sqrt(s_NN) = " << sqrtsNN_ << ")" << std::endl;
  std::cout << " beam rapidity is " << beam_y_ << std::endl;
  std::cout << "TimeSlice length : " << ts_length_ << std::endl;

  for (const auto& mod_pos : modules_pos_) {
    mod_pos.Print();
  }
}

void DataHeader::SetBeamMomentum(float mom, float m_target, float m_beam) {
  beam_mom_ = mom;
  const float energy_lab = sqrt(m_beam * m_beam + beam_mom_ * beam_mom_);
  sqrtsNN_ = sqrt(m_target * m_target + m_beam * m_beam + 2 * m_target * energy_lab);
  const float E = sqrtsNN_ / 2;
  const float p = sqrt(E * E - m_target * m_target);
  beam_y_ = log((E + p) / (E - p)) / 2.;
}

ModulePositions& DataHeader::AddDetector() {
  modules_pos_.emplace_back(modules_pos_.size());
  return modules_pos_.back();
}

}// namespace AnalysisTree
