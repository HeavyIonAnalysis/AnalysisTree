/* Copyright (C) 2019-2021 GSI, Universität Tübingen, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Eugeny Kashirin, Ilya Selyuzhenkov */
#include "EventHeader.hpp"

#include <TVector3.h>

namespace AnalysisTree {

TVector3 EventHeader::GetVertexPosition3() const {
  return TVector3(vtx_pos_[Exyz::kX], vtx_pos_[Exyz::kY], vtx_pos_[Exyz::kZ]);
}

void EventHeader::SetVertexPosition3(const TVector3& pos) {
  vtx_pos_[Exyz::kX] = pos.Px();
  vtx_pos_[Exyz::kY] = pos.Py();
  vtx_pos_[Exyz::kZ] = pos.Pz();
}

const EventHeader& EventHeader::GetChannel(size_t i) const {
  if (i == 0) {
    return *this;
  }
  throw std::out_of_range("EventHeader::GetChannel - Input " + std::to_string(i) + " !=0");
}

EventHeader& EventHeader::Channel(size_t i) {
  if (i == 0) {
    return *this;
  }
  throw std::out_of_range("EventHeader::GetChannel - Input " + std::to_string(i) + " !=0");
}

void EventHeader::Print() const noexcept {
  std::cout << vtx_pos_[Exyz::kX] << " " << vtx_pos_[Exyz::kX] << " " << vtx_pos_[Exyz::kX] << std::endl;
  Container::Print();
}

}// namespace AnalysisTree