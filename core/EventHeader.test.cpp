/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_CORE_EVENTHEADER_TEST_HPP_
#define ANALYSISTREE_CORE_EVENTHEADER_TEST_HPP_

#include <gtest/gtest.h>

#include <TVector3.h>

#include "EventHeader.hpp"

namespace {

using namespace AnalysisTree;

TEST(EventHeader, Basics) {

  EventHeader event_header;

  EXPECT_EQ(event_header.GetSize<int>(), 0);
  EXPECT_EQ(event_header.GetSize<float>(), 0);
  EXPECT_EQ(event_header.GetSize<bool>(), 0);

  event_header.SetVertexPosition3({1., 2., 3.});

  EXPECT_FLOAT_EQ(event_header.GetVertexX(), 1.);
  EXPECT_FLOAT_EQ(event_header.GetVertexY(), 2.);
  EXPECT_FLOAT_EQ(event_header.GetVertexZ(), 3.);
}
}// namespace

#endif//ANALYSISTREE_CORE_EVENTHEADER_TEST_HPP_
