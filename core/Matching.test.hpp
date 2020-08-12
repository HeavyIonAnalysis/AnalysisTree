#ifndef ANALYSISTREE_CORE_MATCHING_TEST_HPP_
#define ANALYSISTREE_CORE_MATCHING_TEST_HPP_

#include <gtest/gtest.h>

#include "Matching.hpp"

namespace {

using namespace AnalysisTree;

TEST(Test_AnalysisTreeCore, Test_Matching) {

  Matching matching(0, 1);

  ASSERT_EQ(matching.GetBranch1Id(), 0);
  ASSERT_EQ(matching.GetBranch2Id(), 1);
  ASSERT_EQ(matching.GetMatches().size(), 0);

  matching.AddMatch(0,0);
  matching.AddMatch(1,2);

  ASSERT_EQ(matching.GetMatches().size(), 2);

  ASSERT_EQ(matching.GetMatch(0), 0);
  ASSERT_EQ(matching.GetMatch(1), 2);
  ASSERT_EQ(matching.GetMatch(2), UndefValueInt);

  ASSERT_EQ(matching.GetMatch(0, true), 0);
  ASSERT_EQ(matching.GetMatch(2, true), 1);
  ASSERT_EQ(matching.GetMatch(1, true), UndefValueInt);
}


}// namespace

#endif //ANALYSISTREE_CORE_MATCHING_TEST_HPP_
