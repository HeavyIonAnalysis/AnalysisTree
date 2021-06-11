/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "Particle.hpp"

namespace AnalysisTree {

void Particle::SetPid(PdgCode_t pid) {
  pid_ = pid;
  if (mass_ == -1000.f)
    mass_ = GetMassByPdgId(pid);
}
}// namespace AnalysisTree