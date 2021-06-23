/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_CORE_MATCHING_TEST_HPP_
#define ANALYSISTREE_CORE_MATCHING_TEST_HPP_

#include <gtest/gtest.h>

#include "Matching.hpp"

namespace {

using namespace AnalysisTree;

TEST(Matching, Basics) {

  Matching matching(0, 1);

  ASSERT_EQ(matching.GetBranch1Id(), 0);
  ASSERT_EQ(matching.GetBranch2Id(), 1);
  ASSERT_EQ(matching.GetMatches().size(), 0);

  matching.AddMatch(0, 0);
  matching.AddMatch(1, 2);

  ASSERT_EQ(matching.GetMatches().size(), 2);

  ASSERT_EQ(matching.GetMatch(0), 0);
  ASSERT_EQ(matching.GetMatch(1), 2);
  ASSERT_EQ(matching.GetMatch(2), UndefValueInt);

  ASSERT_EQ(matching.GetMatch(0, true), 0);
  ASSERT_EQ(matching.GetMatch(2, true), 1);
  ASSERT_EQ(matching.GetMatch(1, true), UndefValueInt);
}

}// namespace

#endif//ANALYSISTREE_CORE_MATCHING_TEST_HPP_
