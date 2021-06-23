/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_CORE_BRANCHCONFIG_TEST_H_
#define ANALYSISTREE_CORE_BRANCHCONFIG_TEST_H_

#include <gtest/gtest.h>

#include "BranchConfig.hpp"

namespace {

using namespace AnalysisTree;

TEST(BranchConfig, Basics) {

  auto det_types = {DetType::kTrack, DetType::kModule, DetType::kParticle, DetType::kHit, DetType::kEventHeader};
  for (auto det_type : det_types) {
    BranchConfig branch_config("RecTrack", det_type);
    branch_config.AddField<float>("test_f");
    branch_config.AddField<int>("test_i");
    branch_config.AddField<bool>("test_b");

    EXPECT_EQ(branch_config.GetFieldId("test_i"), 0);
    EXPECT_EQ(branch_config.GetFieldId("test_b"), 0);
    EXPECT_EQ(branch_config.GetFieldId("test_f"), 0);

    EXPECT_EQ(branch_config.GetFieldType("test_f"), Types::kFloat);
    EXPECT_EQ(branch_config.GetFieldType("test_i"), Types::kInteger);
    EXPECT_EQ(branch_config.GetFieldType("test_b"), Types::kBool);
  }

  BranchConfig branch_config("RecTrack", DetType::kTrack);

  EXPECT_EQ(branch_config.GetFieldId("pT"), TrackFields::kPt);
  EXPECT_EQ(branch_config.GetFieldId("phi"), TrackFields::kPhi);
  EXPECT_EQ(branch_config.GetFieldId("eta"), TrackFields::kEta);
  EXPECT_EQ(branch_config.GetFieldId("p"), TrackFields::kP);
  EXPECT_EQ(branch_config.GetFieldId("px"), TrackFields::kPx);
  EXPECT_EQ(branch_config.GetFieldId("py"), TrackFields::kPy);
  EXPECT_EQ(branch_config.GetFieldId("pz"), TrackFields::kPz);
}

}// namespace

#endif//ANALYSISTREE_CORE_BRANCHCONFIG_TEST_H_
