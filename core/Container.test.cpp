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
  config.AddField<float>("test_f", "just a test field");
  config.AddField<int>("test_i", "just a test field");
  config.AddField<bool>("test_b", "just a test field");

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

TEST(Container, Double) {
  Container container;
  EXPECT_EQ(container.GetSize<double>(), 0);

  BranchConfig config("RecTrack", DetType::kTrack);
  config.AddField<double>("test_d", "just a test field");
  config.AddField<float>("test_f", "just a test field");

  EXPECT_EQ(config.GetFieldType("test_d"), Types::kDouble);
  EXPECT_EQ(config.GetFieldId("test_d"), 0);
  EXPECT_EQ(config.GetFieldId("test_f"), 0);

  container.Init(config);
  EXPECT_EQ(container.GetSize<double>(), 1);

  // value which cannot be represented exactly in float precision
  const double value = 1. + 1e-12;
  container.SetField(value, 0);
  EXPECT_EQ(container.GetField<double>(0), value);
  EXPECT_NE(static_cast<float>(value), value);

  auto clone = config.Clone("RecTrackClone", DetType::kTrack);
  EXPECT_EQ(clone.GetFieldType("test_d"), Types::kDouble);
  EXPECT_EQ(clone.GetSize<double>(), 1);

  config.RemoveField("test_d");
  EXPECT_FALSE(config.HasField("test_d"));
}

}// namespace

#endif//ANALYSISTREE_CORE_CONTAINER_TEST_HPP_
