/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_INFRA_BRANCH_TEST_CPP_
#define ANALYSISTREE_INFRA_BRANCH_TEST_CPP_

#include <gtest/gtest.h>

#include "TaskManager.hpp"
#include "ToyMC.hpp"

#include "Branch.hpp"

#include <random>

namespace {

using namespace AnalysisTree;

TEST(Branch, Basics) {

  RunToyMC(1000);
  Chain t("toymc_analysis_task.root", "tTree");
  t.InitPointersToBranches({"SimParticles"});

  auto config = t.GetConfiguration();
  const auto& ptrs = t.GetBranchPointers();



}

}// namespace


#endif//ANALYSISTREE_INFRA_BRANCH_TEST_CPP_
