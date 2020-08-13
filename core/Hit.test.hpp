#ifndef ANALYSISTREE_TEST_CORE_HIT_TEST_HPP_
#define ANALYSISTREE_TEST_CORE_HIT_TEST_HPP_

#include <gtest/gtest.h>

#include "Hit.hpp"

namespace {

using namespace AnalysisTree;

TEST(Test_AnalysisTreeCore, Test_Hit) {

  Hit hit;

  EXPECT_EQ(hit.GetSize<int>(), 0);
  EXPECT_EQ(hit.GetSize<float>(), 0);
  EXPECT_EQ(hit.GetSize<bool>(), 0);

  hit.SetPosition({1., 2., 3.});
  hit.SetSignal(10.);

  EXPECT_EQ(hit.GetSize<int>(), 0);
  EXPECT_EQ(hit.GetSize<float>(), 0);
  EXPECT_EQ(hit.GetSize<bool>(), 0);

  EXPECT_FLOAT_EQ(hit.GetX(), 1.);
  EXPECT_FLOAT_EQ(hit.GetY(), 2.);
  EXPECT_FLOAT_EQ(hit.GetZ(), 3.);
  EXPECT_FLOAT_EQ(hit.GetPhi(), atan2(2., 1.));
  EXPECT_FLOAT_EQ(hit.GetSignal(), 10.);

  EXPECT_FLOAT_EQ(hit.GetX(), hit.GetField<float>(HitFields::kX));
  EXPECT_FLOAT_EQ(hit.GetY(), hit.GetField<float>(HitFields::kY));
  EXPECT_FLOAT_EQ(hit.GetZ(), hit.GetField<float>(HitFields::kZ));
  EXPECT_FLOAT_EQ(hit.GetPhi(), hit.GetField<float>(HitFields::kPhi));
  EXPECT_FLOAT_EQ(hit.GetSignal(), hit.GetField<float>(HitFields::kSignal));
}
}// namespace

#endif//ANALYSISTREE_TEST_CORE_HIT_TEST_HPP_
