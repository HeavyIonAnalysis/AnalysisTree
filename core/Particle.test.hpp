#ifndef ANALYSISTREE_CORE_PARTICLE_TEST_HPP_
#define ANALYSISTREE_CORE_PARTICLE_TEST_HPP_

#include <gtest/gtest.h>

#include "Particle.hpp"

namespace{

using namespace AnalysisTree;

TEST(Test_AnalysisTreeCore, Test_Particle) {

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
  EXPECT_FLOAT_EQ(particle.GetMass(), 0.13957);
}
}

#endif //ANALYSISTREE_CORE_PARTICLE_TEST_HPP_
