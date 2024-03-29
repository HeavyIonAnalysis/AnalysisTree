#ifndef ANALYSISTREE_INFRA_FIELD_TEST_HPP_
#define ANALYSISTREE_INFRA_FIELD_TEST_HPP_

#include <gtest/gtest.h>

#include <infra-1.0/Field.hpp>

#include <core/BranchConfig.hpp>
#include <core/Configuration.hpp>
#include <core/Track.hpp>

namespace {

using namespace AnalysisTree::Version1;

TEST(Field, Basics) {

  AnalysisTree::BranchConfig branch_config("RecTrack", AnalysisTree::DetType::kTrack);
  branch_config.AddField<float>("test_f");
  branch_config.AddField<int>("test_i");
  branch_config.AddField<bool>("test_b");

  AnalysisTree::Configuration configuration;
  configuration.AddBranchConfig(branch_config);

  Field field_f("RecTrack", "test_f");
  Field field_i("RecTrack", "test_i");
  Field field_b("RecTrack", "test_b");

  EXPECT_STREQ(field_f.GetName().c_str(), "test_f");
  EXPECT_STREQ(field_f.GetBranchName().c_str(), "RecTrack");

  field_f.Init(configuration);
  field_i.Init(configuration);
  field_b.Init(configuration);

  EXPECT_EQ(0, field_f.GetFieldId());
  //  EXPECT_EQ(0, field_f.GetBranchId());
  EXPECT_EQ(AnalysisTree::Types::kFloat, field_f.GetFieldType());
  EXPECT_EQ(AnalysisTree::Types::kInteger, field_i.GetFieldType());
  EXPECT_EQ(AnalysisTree::Types::kBool, field_b.GetFieldType());

  AnalysisTree::Track track;
  track.Init(branch_config);
  track.SetField(99.f, 0);
  track.SetField(99, 0);
  track.SetField(true, 0);

  EXPECT_FLOAT_EQ(99.f, field_f.GetValue(track));
  EXPECT_EQ(99, field_i.GetValue(track));
  EXPECT_EQ(true, field_b.GetValue(track));
}

}// namespace

#endif//ANALYSISTREE_INFRA_FIELD_TEST_HPP_
