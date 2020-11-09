#ifndef ANALYSISTREE_CORE_EVENTHEADER_TEST_HPP_
#define ANALYSISTREE_CORE_EVENTHEADER_TEST_HPP_

#include <gtest/gtest.h>
#include "EventHeader.hpp"

namespace {

using namespace AnalysisTree;

TEST(Test_AnalysisTreeCore, Test_EventHeader) {

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

#endif //ANALYSISTREE_CORE_EVENTHEADER_TEST_HPP_
