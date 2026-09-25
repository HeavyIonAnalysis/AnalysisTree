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
    branch_config.AddField<float>("test_f", "just a test field");
    branch_config.AddField<int>("test_i", "just a test field");
    branch_config.AddField<bool>("test_b", "just a test field");

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

TEST(BranchConfig, RemoveField) {
  BranchConfig branch_config("RecTrack", DetType::kTrack);
  branch_config.AddFields<float>({"f0", "f1", "f2"}, "just a test field");
  EXPECT_EQ(branch_config.GetSize<float>(), 3);

  branch_config.RemoveField("f1");
  EXPECT_FALSE(branch_config.HasField("f1"));
  EXPECT_EQ(branch_config.GetSize<float>(), 2);
  EXPECT_EQ(branch_config.GetFieldId("f0"), 0);
  EXPECT_EQ(branch_config.GetFieldId("f2"), 1);

  // a field added after removal must get the next free id, not collide with existing ones
  branch_config.AddField<float>("f3", "just a test field");
  EXPECT_EQ(branch_config.GetFieldId("f3"), 2);
  EXPECT_EQ(branch_config.GetSize<float>(), 3);

  EXPECT_THROW(branch_config.RemoveField("pT"), std::runtime_error);
  EXPECT_EQ(branch_config.GetSize<float>(), 3);
}

}// namespace

#endif//ANALYSISTREE_CORE_BRANCHCONFIG_TEST_H_
