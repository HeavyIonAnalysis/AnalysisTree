
#ifndef ANALYSISTREE_INFRA_PLAINTREEFILLER_TEST_HPP_
#define ANALYSISTREE_INFRA_PLAINTREEFILLER_TEST_HPP_

#include <gtest/gtest.h>

#include "PlainTreeFiller.hpp"
#include "TaskManager.hpp"

TEST(Test_AnalysisTreeInfra, Test_PlainTreeFiller) {

  auto* man = AnalysisTree::TaskManager::GetInstance();
  man->SetOutputName("test_plain_tree.root", "pTree");

  auto* plain_tree = new PlainTreeFiller;
  plain_tree->AddBranch("SimParticles");
  plain_tree->FillBranchNames(); //TODO remove?
  man->AddTask(plain_tree);

  man->Init({"fl_toy_mc.txt"}, {"tTree"});
  man->Run(-1);
  man->Finish();
}




#endif //ANALYSISTREE_INFRA_PLAINTREEFILLER_TEST_HPP_
