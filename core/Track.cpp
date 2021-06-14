/* Copyright (C) 2019-2021 GSI, Universität Tübingen, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Eugeny Kashirin, Ilya Selyuzhenkov */
#include "Track.hpp"

#include <TDatabasePDG.h>

#include <iostream>

namespace AnalysisTree {

void Track::Print() const noexcept {
  std::cout << " Px = " << px_ << "  Py = " << py_ << "  Pz = " << pz_
            << "  phi = " << GetPhi() << "  pT = " << GetPt() << "  eta = " << GetEta() << std::endl;
  Container::Print();
}

bool operator==(const Track& that, const Track& other) noexcept {
  if (&that == &other) {
    return true;
  }
  if ((Container&) that != (Container&) other) {
    return false;
  }
  return that.px_ == other.px_ && that.py_ == other.py_ && that.pz_ == other.pz_;
}

float Track::GetMassByPdgId(PdgCode_t pdg) {

  if (pdg > 1000000000) {//100ZZZAAA0
    auto A = (pdg % 10000) / 10;
    return A * 0.938f /* GeV */;
  }
  auto db = TDatabasePDG::Instance();
  auto particle = db->GetParticle(pdg);

  if (particle) {
    return particle->Mass();
  } else {
    throw std::runtime_error("Mass of " + std::to_string(pdg) + " is not known");
  }
}

}// namespace AnalysisTree