/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include <iostream>

#include <AnalysisTree/Branch.hpp>
#include <AnalysisTree/ToyMC.hpp>

#include "UserTaskRead.hpp"
#include "UserTaskWrite.hpp"

using namespace AnalysisTree;

int main(int argc, char* argv[]) {

  BranchConfig config("test", DetType::kParticle);
  auto* particles = new Particles(1);

  Branch branch(config, particles);
  branch.SetMutable();
  Field pT = branch.GetField("pT");
  Field px = branch.GetField("px");
  Field py = branch.GetField("py");
  Field pz = branch.GetField("pz");

  auto ch = branch.NewChannel();
  ch.SetValue(px, 0.1);
  ch.SetValue(py, 0.3);
  ch.SetValue(pz, 0.5);
}
