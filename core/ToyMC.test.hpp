#ifndef ANALYSISTREE_CORE_TOYMC_TEST_HPP_
#define ANALYSISTREE_CORE_TOYMC_TEST_HPP_

#include <gtest/gtest.h>

#include "ToyMC.hpp"

TEST(Test_AnalysisTreeCore, Test_ToyMC) {
  ToyMC<std::default_random_engine> toy_mc;
  toy_mc.GenerateEvents(1000);
  toy_mc.WriteToFile("toy_mc.root");
}

#endif //ANALYSISTREE_CORE_TOYMC_TEST_HPP_
