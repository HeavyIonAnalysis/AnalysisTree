#ifndef ANALYSISTREE_INFRA_VARIABLE_TEST_HPP_
#define ANALYSISTREE_INFRA_VARIABLE_TEST_HPP_

#include <gtest/gtest.h>

#include <core/Track.hpp>
#include <infra-1.0/Variable.hpp>

#include <core/BranchConfig.hpp>
#include <core/Configuration.hpp>


namespace{

using namespace AnalysisTree::Version1;

TEST(Variable, FromString) {

  auto v1 = Variable::FromString("branch.field");
  EXPECT_EQ(v1.GetBranchName(), "branch");
  EXPECT_EQ(v1.GetFields()[0].GetName(), "field");

  auto v2 = Variable::FromString("branch/field");
  EXPECT_EQ(v1.GetBranchName(), "branch");
  EXPECT_EQ(v1.GetFields()[0].GetName(), "field");

  EXPECT_THROW(Variable::FromString("field"), std::runtime_error);


}

TEST(Variable, Basics) {

  AnalysisTree::BranchConfig branch_config("RecTrack", AnalysisTree::DetType::kTrack);
  branch_config.AddField<float>("test_f");
  branch_config.AddField<int>("test_i");
  branch_config.AddField<bool>("test_b");

  AnalysisTree::Configuration configuration;
  configuration.AddBranchConfig(branch_config);

  Variable var1("RecTrack", "test_f");
  Variable var2("var2", {{"RecTrack", "test_f"}, {"RecTrack", "test_i"}}, [](std::vector<double> var) { return var[0]/var[1]; });

  var1.Init(configuration);
  var2.Init(configuration);

  AnalysisTree::Track track;
  track.Init(branch_config);
  track.SetField(99.f, 0);
  track.SetField(99, 0);
  track.SetField(true, 0);

  EXPECT_FLOAT_EQ(var1.GetValue(track), 99.f);
  EXPECT_FLOAT_EQ(var2.GetValue(track), 1.f);
}

}

#endif //ANALYSISTREE_INFRA_VARIABLE_TEST_HPP_
