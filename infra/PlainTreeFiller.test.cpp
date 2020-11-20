
#ifndef ANALYSISTREE_INFRA_PLAINTREEFILLER_TEST_HPP_
#define ANALYSISTREE_INFRA_PLAINTREEFILLER_TEST_HPP_

#include <gtest/gtest.h>

#include "TaskManager.hpp"
#include "PlainTreeFiller.hpp"

namespace {

using namespace AnalysisTree;

TEST(PlainTreeFiller, Basics) {

  TaskManager* man = TaskManager::GetInstance();

  auto* plain_tree = new PlainTreeFiller;
  plain_tree->AddBranch("SimParticles");
  man->AddTask(plain_tree);

  man->Init({"fl_toy_mc.txt"}, {"tTree"});
  man->Run(-1);
  man->Finish();
}

}


#endif //ANALYSISTREE_INFRA_PLAINTREEFILLER_TEST_HPP_
