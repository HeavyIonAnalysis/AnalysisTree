
#ifndef ANALYSISTREE_INFRA_PLAINTREEFILLER_TEST_HPP_
#define ANALYSISTREE_INFRA_PLAINTREEFILLER_TEST_HPP_

#include <gtest/gtest.h>

#include "PlainTreeFiller.hpp"
#include "TaskManager.hpp"
#include "ToyMC.hpp"

namespace {

using namespace AnalysisTree;

TEST(PlainTreeFiller, Basics) {

  const int n_events = 1000;// TODO propagate somehow
  std::string filename = "toymc_analysis_task.root";
  std::string treename = "tTree";
  std::string filelist = "fl_toy_mc.txt";

  auto* man = TaskManager::GetInstance();

  auto* toy_mc = new ToyMC<std::default_random_engine>;
  man->AddTask(toy_mc);
  man->SetOutputName(filename, treename);

  man->Init();
  man->Run(n_events);
  man->Finish();

  man->ClearTasks();

  std::ofstream fl(filelist);
  fl << filename << "\n";
  fl.close();

  auto* plain_tree = new PlainTreeFiller;
  plain_tree->AddBranch("SimParticles");
  man->AddTask(plain_tree);

  man->Init({"fl_toy_mc.txt"}, {"tTree"});
  man->Run(-1);
  man->Finish();
}

}// namespace

#endif//ANALYSISTREE_INFRA_PLAINTREEFILLER_TEST_HPP_
