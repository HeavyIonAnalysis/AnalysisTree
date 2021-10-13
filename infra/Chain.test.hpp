/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_INFRA_CHAIN_TEST_HPP_
#define ANALYSISTREE_INFRA_CHAIN_TEST_HPP_

#include <gtest/gtest.h>

#include <infra/Chain.hpp>

namespace {

using namespace AnalysisTree;

TEST(Test_AnalysisTreeInfra, Test_Chain) {

  auto* chain = new Chain({"fl_toy_mc.txt"}, {"tTree"});

  EXPECT_EQ(chain->GetNtrees(), 1);
  EXPECT_EQ(chain->GetEntries(), 1000);

  chain->InitPointersToBranches({});
  auto branches_map = chain->GetBranchPointers();

  auto n_px = chain->Draw("SimParticles.px", "");
  EXPECT_GT(n_px, 0);

  auto* particles = std::get<Particles*>(chain->GetPointerToBranch("SimParticles"));
  auto* tracks = std::get<TrackDetector*>(chain->GetPointerToBranch("RecTracks"));

  for (ULong64_t i = 0; i < 10; ++i) {
    chain->GetEntry(i);

    const auto Ms = particles->GetNumberOfChannels();
    const auto Mr = tracks->GetNumberOfChannels();

    EXPECT_GT(Mr, 0);
    EXPECT_GT(Ms, 0);
  }
}

}// namespace

#endif//ANALYSISTREE_INFRA_CHAIN_TEST_HPP_
