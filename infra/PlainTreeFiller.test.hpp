
#ifndef ANALYSISTREE_INFRA_PLAINTREEFILLER_TEST_HPP_
#define ANALYSISTREE_INFRA_PLAINTREEFILLER_TEST_HPP_

#include <gtest/gtest.h>

#include "PlainTreeFiller.hpp"

TEST(Test_AnalysisTreeInfra, Test_PlainTreeFiller) {


  TaskManager man({"fl_toy_mc.txt"}, {"tTree"});
  man.SetOutFileName("test_plain_tree.root");

  auto* plain_tree = new PlainTreeFiller;
  plain_tree->AddBranch("SimParticles");
//  plain_tree->FillBranchNames(); //TODO remove?
  man.AddTask(plain_tree);

  man.Init();
  man.Run(-1);
  man.Finish();
}




#endif //ANALYSISTREE_INFRA_PLAINTREEFILLER_TEST_HPP_
