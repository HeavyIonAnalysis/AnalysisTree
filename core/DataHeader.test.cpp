/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_CORE_DATAHEADER_TEST_HPP_
#define ANALYSISTREE_CORE_DATAHEADER_TEST_HPP_

#include <gtest/gtest.h>

#include "DataHeader.hpp"

namespace {

using namespace AnalysisTree;

TEST(DataContainer, Basics) {

  DataHeader dh;
  dh.SetBeamMomentum(10.);

  EXPECT_FLOAT_EQ(dh.GetBeamRapidity(), 1.530965);
}

}// namespace

#endif//ANALYSISTREE_CORE_DATAHEADER_TEST_HPP_
