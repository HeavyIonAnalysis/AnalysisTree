/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_INFRA_BRANCH_TEST_CPP_
#define ANALYSISTREE_INFRA_BRANCH_TEST_CPP_

#include <gtest/gtest.h>

#include "Branch.hpp"

namespace {

using namespace AnalysisTree;

TEST(Branch, Basics) {
  BranchConfig config("test", DetType::kParticle);
  auto* particles = new Particles(1);
  auto& particle = particles->AddChannel(config);
  particle.SetMomentum(0.1, 0.3, 0.5);
  particle.SetPid(211);

  Branch branch(config, particles);

  EXPECT_EQ(branch.GetId(), 1);
  EXPECT_EQ(branch.GetBranchType(), DetType::kParticle);
  EXPECT_EQ(branch.GetDataRaw<Particles*>(), particles);
  EXPECT_EQ(branch.size(), 1);

  EXPECT_NO_THROW(branch.CheckFrozen(false));
  EXPECT_ANY_THROW(branch.CheckFrozen(true));

  EXPECT_NO_THROW(branch.CheckMutable(false));
  EXPECT_ANY_THROW(branch.CheckMutable(true));

  branch.SetMutable(true);
  branch.Freeze(true);

  EXPECT_NO_THROW(branch.CheckFrozen(true));
  EXPECT_ANY_THROW(branch.CheckFrozen(false));

  EXPECT_NO_THROW(branch.CheckMutable(true));
  EXPECT_ANY_THROW(branch.CheckMutable(false));
}

TEST(Branch, ReadValue) {
  BranchConfig config("test", DetType::kParticle);
  auto* particles = new Particles(1);
  auto& particle = particles->AddChannel(config);
  particle.SetMomentum(0.1, 0.3, 0.5);
  particle.SetPid(211);

  Branch branch(config, particles);

  Field pT = branch.GetField("pT");
  Field px = branch.GetField("px");
  Field py = branch.GetField("py");
  Field pz = branch.GetField("pz");

  auto channel = branch[0];
  EXPECT_FLOAT_EQ(channel[pT], sqrt(0.1*0.1+0.3*0.3));
  EXPECT_FLOAT_EQ(channel[px], 0.1);
  EXPECT_FLOAT_EQ(channel[py], 0.3);
  EXPECT_FLOAT_EQ(channel[pz], 0.5);
}

TEST(Branch, WriteValue) {
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

  auto particle = ch.Data<Particle>();
  EXPECT_FLOAT_EQ(particle->GetPx(), 0.1);
  EXPECT_FLOAT_EQ(particle->GetPy(), 0.3);
  EXPECT_FLOAT_EQ(particle->GetPz(), 0.5);
}



}
#endif//ANALYSISTREE_INFRA_BRANCH_TEST_CPP_

