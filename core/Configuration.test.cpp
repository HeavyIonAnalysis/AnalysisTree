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
  branch_config.AddField<float>("test_f");
  branch_config.AddField<int>("test_i");
  branch_config.AddField<bool>("test_b");

  config.AddBranchConfig(branch_config);

  EXPECT_EQ(config.GetNumberOfBranches(), 1);

  const auto& br1 = config.GetBranchConfig("RecTrack");
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

TEST(Configuration, Write){

  auto* config = new Configuration("test");

  auto rec = BranchConfig("RecTrack", DetType::kParticle);
  auto sim = BranchConfig("SimTrack", DetType::kParticle);

  config->AddBranchConfig(rec);
  config->AddBranchConfig(sim);

  auto* match = new Matching(rec.GetId(), sim.GetId());
  config->AddMatch(match);

  auto* file = TFile::Open("Test_WriteConfiguration.root", "recreate");
  config->Write("Configuration");
  file->Close();

//  delete file;
//  delete match;
//  delete config;

  file = TFile::Open("Test_WriteConfiguration.root", "read");
  config = file->Get<Configuration>("Configuration");
  config->Print();

}


}// namespace

#endif//ANALYSISTREE_CORE_CONFIGURATION_TEST_HPP_
