/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_CORE_CONFIGURATION_TEST_HPP_
#define ANALYSISTREE_CORE_CONFIGURATION_TEST_HPP_

#include <TFile.h>
#include <gtest/gtest.h>

#include "Configuration.hpp"
#include "Matching.hpp"

namespace {

using namespace AnalysisTree;

TEST(Configuration, Basics) {

  Configuration config("test");

  EXPECT_EQ(config.GetNumberOfBranches(), 0);
  EXPECT_EQ(config.GetMatches().size(), 0);

  BranchConfig branch_config("RecTrack", DetType::kTrack);
  branch_config.AddField<float>("test_f", "just a test field");
  branch_config.AddField<int>("test_i", "just a test field");
  branch_config.AddField<bool>("test_b", "just a test field");

  config.AddBranchConfig(branch_config);

  EXPECT_EQ(config.GetNumberOfBranches(), 1);
  //  EXPECT_EQ(config.GetLastId(), 0);

  const auto& br1 = config.GetBranchConfig("RecTrack");
  //  EXPECT_EQ(br1.GetId(), 0);
}

TEST(Configuration, Match) {
  Configuration config("test");

  auto rec = BranchConfig("RecTrack", DetType::kParticle);
  auto sim = BranchConfig("SimTrack", DetType::kParticle);

  config.AddBranchConfig(rec);
  config.AddBranchConfig(sim);

  Matching match(rec.GetId(), sim.GetId());
  config.AddMatch(&match);

  EXPECT_STREQ(config.GetMatchName("RecTrack", "SimTrack").c_str(), "RecTrack2SimTrack");

  try {
    auto name = config.GetMatchName("SimTrack", "RecTrack");
    FAIL() << "throwException() should throw an error\n";
  } catch (std::runtime_error& exception) {}

  auto match_info = config.GetMatchInfo("RecTrack", "SimTrack");
  auto match_info_inv = config.GetMatchInfo("SimTrack", "RecTrack");

  EXPECT_EQ(match_info.second, false);
  EXPECT_EQ(match_info_inv.second, true);

  EXPECT_STREQ(match_info.first.c_str(), "RecTrack2SimTrack");
  EXPECT_STREQ(match_info_inv.first.c_str(), "RecTrack2SimTrack");
}

TEST(Configuration, RemoveBranch) {
  Configuration config("test");

  auto rec = BranchConfig("RecTrack", DetType::kParticle);
  auto sim = BranchConfig("SimTrack", DetType::kParticle);

  config.AddBranchConfig(rec);
  config.AddBranchConfig(sim);

  Matching match(rec.GetId(), sim.GetId());
  config.AddMatch(&match);

  config.RemoveBranchConfig("SimTrack");
  EXPECT_EQ(config.GetNumberOfBranches(), 1);
  EXPECT_EQ(config.GetMatches().size(), 0);
}

TEST(Configuration, ReadWrite) {
  Configuration config("c");
  config.AddBranchConfig(BranchConfig("test1", AnalysisTree::DetType::kParticle));
  config.AddBranchConfig(BranchConfig("test2", AnalysisTree::DetType::kParticle));
  auto matching_to_add = new Matching(
      config.GetBranchConfig("test1").GetId(),
      config.GetBranchConfig("test2").GetId());
  auto matching_swapped_branches = new Matching(
      config.GetBranchConfig("test2").GetId(),
      config.GetBranchConfig("test1").GetId());
  EXPECT_NO_THROW(config.AddMatch(matching_to_add));
  //  Not yet implemented?
  //  EXPECT_THROW(config.AddMatch(matching_to_add), std::runtime_error); // Attempt to add existing branch
  //  EXPECT_THROW(config.AddMatch(matching_swapped_branches), std::runtime_error); // Attempt to add existing branch

  {
    TFile f("configuration_io.root", "recreate");
    config.Write("Configuration");
    f.Close();
  }

  {
    TFile f("configuration_io.root", "read");
    auto config_new = f.Get<Configuration>("Configuration");
    ASSERT_TRUE(config_new);
  }

  delete matching_to_add;
  delete matching_swapped_branches;
}

}// namespace

#endif//ANALYSISTREE_CORE_CONFIGURATION_TEST_HPP_
