/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_INFRA_SIMPLECUT_TEST_HPP_
#define ANALYSISTREE_INFRA_SIMPLECUT_TEST_HPP_

#include "SimpleCut.hpp"
#include <gtest/gtest.h>

#include <vector>

namespace {

using namespace AnalysisTree;

TEST(SimpleCut, Basics) {
  SimpleCut ecut1 = EqualsCut(Variable::FromString("BranchName.FieldName"), 1);
  SimpleCut ecut1same = EqualsCut(Variable::FromString("BranchName.FieldName"), 1);
  SimpleCut ecut2 = EqualsCut(Variable::FromString("BranchName.FieldName"), 2);
  SimpleCut ecut3 = EqualsCut(Variable::FromString("BranchName3.FieldName3"), 1);
  SimpleCut ecut1copy = ecut1;
  SimpleCut& ecut1ref = ecut1;

  EXPECT_TRUE(ecut1 == ecut1);
  EXPECT_TRUE(ecut1 == ecut1same);
  EXPECT_TRUE(ecut1 == ecut1copy);
  EXPECT_TRUE(ecut1 == ecut1ref);
  EXPECT_FALSE(ecut1 == ecut2);
  EXPECT_FALSE(ecut1 == ecut3);
  EXPECT_FALSE(ecut2 == ecut3);
  
  SimpleCut rcut1 = RangeCut(Variable::FromString("BranchName.FieldName"), 1, 2);
  SimpleCut rcut1same = RangeCut(Variable::FromString("BranchName.FieldName"), 1, 2);
  SimpleCut rcut2 = RangeCut(Variable::FromString("BranchName.FieldName"), 2, 5);
  SimpleCut rcut3 = RangeCut(Variable::FromString("BranchName3.FieldName3"), 1, 2);
  SimpleCut rcut1copy = rcut1;
  SimpleCut& rcut1ref = rcut1;
  
  EXPECT_TRUE(rcut1 == rcut1);
  EXPECT_TRUE(rcut1 == rcut1same);
  EXPECT_TRUE(rcut1 == rcut1copy);
  EXPECT_TRUE(rcut1 == rcut1ref);
  EXPECT_FALSE(rcut1 == rcut2);
  EXPECT_FALSE(rcut1 == rcut3);
  EXPECT_FALSE(rcut2 == rcut3);

  SimpleCut lcut1 = SimpleCut((std::vector<std::string>){"B1.f1", "B2.f2", "B3.f3"}, [](std::vector<double> par){ return par[0]/par[1]*par[2] < 10; });
  SimpleCut lcut1same = SimpleCut((std::vector<std::string>){"B1.f1", "B2.f2", "B3.f3"}, [](std::vector<double> par){ return par[0]/par[1]*par[2] < 10; });
  SimpleCut lcut2 = SimpleCut((std::vector<std::string>){"B11.f11", "B22.f22", "B33.f33"}, [](std::vector<double> par){ return par[0]/par[1]*par[2] < 5; });
  SimpleCut lcut1copy = lcut1;
  SimpleCut& lcut1ref = lcut1;

  EXPECT_TRUE(lcut1 == lcut1);
  EXPECT_FALSE(lcut1 == lcut1same); // it should not be like that, but this is a concession
  EXPECT_FALSE(lcut1 == lcut1copy); // it should not be like that, but this is a concession
  EXPECT_TRUE(lcut1 == lcut1ref);
  EXPECT_FALSE(lcut1 == lcut2);
  EXPECT_FALSE(lcut1 == ecut1);
  EXPECT_FALSE(lcut1 == rcut1);
}

}

#endif//ANALYSISTREE_INFRA_SIMPLECUT_TEST_HPP_
