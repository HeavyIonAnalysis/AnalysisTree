/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_CORE_PARTICLE_TEST_HPP_
#define ANALYSISTREE_CORE_PARTICLE_TEST_HPP_

#include <gtest/gtest.h>

#include "Particle.hpp"

#include "TDatabasePDG.h"

namespace {

using namespace AnalysisTree;

TEST(Particle, Basics) {

  Particle particle;

  EXPECT_FLOAT_EQ(particle.GetSize<int>(), 0);
  EXPECT_FLOAT_EQ(particle.GetSize<float>(), 0);
  EXPECT_FLOAT_EQ(particle.GetSize<bool>(), 0);

  particle.SetMomentum3({1, 2, 3});
  particle.SetPid(211);

  EXPECT_EQ(particle.GetSize<int>(), 0);
  EXPECT_EQ(particle.GetSize<float>(), 0);
  EXPECT_EQ(particle.GetSize<bool>(), 0);

  EXPECT_EQ(particle.GetPid(), 211);
  EXPECT_FLOAT_EQ(particle.GetMass(), TDatabasePDG::Instance()->GetParticle(211)->Mass());
}
}// namespace

#endif//ANALYSISTREE_CORE_PARTICLE_TEST_HPP_
