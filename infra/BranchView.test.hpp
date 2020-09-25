//
// Created by eugene on 25/09/2020.
//

#ifndef ANALYSISTREE_INFRA_BRANCHVIEW_TEST_HPP
#define ANALYSISTREE_INFRA_BRANCHVIEW_TEST_HPP

#include "BranchView.hpp"
#include "BranchConfig.hpp"
#include <gtest/gtest.h>

namespace {

using namespace AnalysisTree;

TEST(Test_AnalysisTreeBranch,Test_GetFields) {
  BranchConfig c("test", DetType::kEventHeader);
  AnalysisTreeBranch<EventHeader> br(c, nullptr);

  EXPECT_EQ(br.GetFields(), std::vector<std::string>({"vtx_x","vtx_y","vtx_z"}));
  EXPECT_EQ(br.Select("vtx_x")->GetFields(), std::vector<std::string>({"vtx_x"}));

  auto s_result = br.Select("vtx_x");
  s_result->GetFields();
}

TEST(Test_AnalysisTreeBranch,Test_GetNChannels) {
  BranchConfig c("test", DetType::kEventHeader);
  AnalysisTreeBranch<EventHeader> br(c);
  EXPECT_EQ(br.GetNumberOfChannels(), 1);

  BranchConfig c1("test", DetType::kTrack);
  AnalysisTreeBranch<TrackDetector> br1(c);
  EXPECT_EQ(br1.GetNumberOfChannels(), 0);
}
}

#endif//ANALYSISTREE_INFRA_BRANCHVIEW_TEST_HPP
