#ifndef ANALYSISTREE_CORE_CONFIGURATION_TEST_HPP_
#define ANALYSISTREE_CORE_CONFIGURATION_TEST_HPP_

#include <gtest/gtest.h>

#include "Configuration.hpp"

namespace {

using namespace AnalysisTree;

TEST(Test_AnalysisTreeCore, Test_Configuration) {

  Configuration config("test");

  EXPECT_EQ(config.GetNumberOfBranches(), 0);
  EXPECT_EQ(config.GetMatches().size(), 0);

  BranchConfig branch_config("RecTrack", DetType::kTrack);
  branch_config.AddField<float>("test_f");
  branch_config.AddField<int>("test_i");
  branch_config.AddField<bool>("test_b");

  config.AddBranchConfig(branch_config);

  EXPECT_EQ(config.GetNumberOfBranches(), 1);
  EXPECT_EQ(config.GetLastId(), 0);

  const auto& br1 = config.GetBranchConfig("RecTrack");
  EXPECT_EQ(br1.GetId(), 0);

//  Matching match()

}


}// namespace


#endif //ANALYSISTREE_CORE_CONFIGURATION_TEST_HPP_
