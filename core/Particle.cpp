/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "Particle.hpp"

namespace AnalysisTree {

void Particle::SetPid(PdgCode_t pid) {
  pid_ = pid;
  if (mass_ == -1000.f)
    mass_ = GetMassByPdgId(pid);

  if (charge_ == -1000) {
    charge_ = GetChargeByPdgId(pid);
  }
}

void Particle::CheckIsAllowedSetMassAndChargeExplicitly() const {
  if (!is_allowed_set_charge_and_mass_explicitly_) {
    std::string message = "Particle::CheckIsAllowedSetMassAndChargeExplicitly(): ";
    message += "mass and charge of the particle are set automatically with SetPid() call ";
    message += "(unless they were already assigned with some values, incl. when copied content from Track to Particle). ";
    message += "Use SetMass() and SetCharge() only if you want to set them different from PDG-true values. ";
    message += "To unblock this possibility use SetIsAllowedSetMassAndChargeExplicitly() function.";
    throw std::runtime_error(message);
  }
}

}// namespace AnalysisTree