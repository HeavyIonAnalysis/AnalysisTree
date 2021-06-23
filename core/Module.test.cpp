/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_TEST_CORE_MODULE_TEST_HPP_
#define ANALYSISTREE_TEST_CORE_MODULE_TEST_HPP_

#include <gtest/gtest.h>

#include "Module.hpp"

namespace {

using namespace AnalysisTree;

TEST(Module, Basics) {

  Module module;

  EXPECT_EQ(module.GetSize<int>(), 0);
  EXPECT_EQ(module.GetSize<float>(), 0);
  EXPECT_EQ(module.GetSize<bool>(), 0);

  module.SetNumber(1);
  module.SetSignal(10.);

  EXPECT_EQ(module.GetSize<int>(), 0);
  EXPECT_EQ(module.GetSize<float>(), 0);
  EXPECT_EQ(module.GetSize<bool>(), 0);

  EXPECT_FLOAT_EQ(module.GetNumber(), 1.);
  EXPECT_FLOAT_EQ(module.GetSignal(), 10.);

  EXPECT_FLOAT_EQ(module.GetNumber(), module.GetField<float>(ModuleFields::kNumber));
  EXPECT_FLOAT_EQ(module.GetSignal(), module.GetField<float>(ModuleFields::kSignal));
}
}// namespace

#endif//ANALYSISTREE_TEST_CORE_MODULE_TEST_HPP_
