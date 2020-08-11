#ifndef ANALYSISTREE_CORE_BRANCHCONFIG_TEST_H_
#define ANALYSISTREE_CORE_BRANCHCONFIG_TEST_H_

#include <gtest/gtest.h>

#include "BranchConfig.hpp"

namespace {

using namespace AnalysisTree;

TEST(Test_AnalysisTreeCore, Test_BranchConfig) {

  AnalysisTree::BranchConfig branch_config("RecTrack", AnalysisTree::DetType::kTrack);
  branch_config.AddField<float>("test_f");
  branch_config.AddField<int>("test_i");
  branch_config.AddField<bool>("test_b");

  EXPECT_EQ(branch_config.GetFieldId("test_i"), 0);
  EXPECT_EQ(branch_config.GetFieldId("test_b"), 0);
  EXPECT_EQ(branch_config.GetFieldId("test_f"), 0);

  EXPECT_EQ(branch_config.GetFieldId("pT"), TrackFields::kPt);
  EXPECT_EQ(branch_config.GetFieldId("phi"), TrackFields::kPhi);
  EXPECT_EQ(branch_config.GetFieldId("eta"), TrackFields::kEta);
  EXPECT_EQ(branch_config.GetFieldId("p"), TrackFields::kP);
  EXPECT_EQ(branch_config.GetFieldId("px"), TrackFields::kPx);
  EXPECT_EQ(branch_config.GetFieldId("py"), TrackFields::kPy);
  EXPECT_EQ(branch_config.GetFieldId("pz"), TrackFields::kPz);
}


}// namespace


#endif //ANALYSISTREE_CORE_BRANCHCONFIG_TEST_H_
