/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_CORE_CONTAINER_TEST_HPP_
#define ANALYSISTREE_CORE_CONTAINER_TEST_HPP_

#include <gtest/gtest.h>

#include "BranchConfig.hpp"
#include "Container.hpp"

namespace {

using namespace AnalysisTree;

TEST(Container, Basics) {

  Container container;

  EXPECT_EQ(container.GetSize<float>(), 0);
  EXPECT_EQ(container.GetSize<int>(), 0);
  EXPECT_EQ(container.GetSize<bool>(), 0);

  BranchConfig config("RecTrack", DetType::kTrack);
  config.AddField<float>("test_f");
  config.AddField<int>("test_i");
  config.AddField<bool>("test_b");

  container.Init(config);

  EXPECT_EQ(container.GetSize<float>(), 1);
  EXPECT_EQ(container.GetSize<int>(), 1);
  EXPECT_EQ(container.GetSize<bool>(), 1);

  container.SetField(1, 0);
  container.SetField(1.f, 0);
  container.SetField(true, 0);

  EXPECT_EQ(container.GetField<float>(0), 1.f);
  EXPECT_EQ(container.GetField<int>(0), 1);
  EXPECT_EQ(container.GetField<bool>(0), true);
}

}// namespace

#endif//ANALYSISTREE_CORE_CONTAINER_TEST_HPP_
