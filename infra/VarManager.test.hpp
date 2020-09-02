#ifndef ANALYSISTREE_INFRA_VARMANAGER_TEST_HPP_
#define ANALYSISTREE_INFRA_VARMANAGER_TEST_HPP_

#include <gtest/gtest.h>

#include "VarManager.hpp"
#include "TaskManager.hpp"

namespace{

using namespace AnalysisTree;

TEST(Test_AnalysisTreeInfra, Test_VarManager) {

  ToyMC<std::default_random_engine> toy_mc;
  toy_mc.GenerateEvents(1000);
  toy_mc.WriteToFile("toy_mc.root", "fl_toy_mc.txt");

  TaskManager man({"fl_toy_mc.txt"}, {"tTree"});

  auto* var_manager = new VarManager;
  Variable pT_sim("SimParticles", "pT");
  Variable pT_rec("RecTracks", "pT");
  Cuts eta_cut("eta_cut", {SimpleCut({"SimParticles", "eta"}, -1, 1)});
  var_manager->AddEntry(VarManagerEntry({pT_sim}));
  var_manager->AddEntry(VarManagerEntry({pT_rec}));
  var_manager->AddEntry(VarManagerEntry({pT_sim, pT_rec}));
  var_manager->AddEntry(VarManagerEntry({pT_sim, pT_rec}, &eta_cut));
  var_manager->FillBranchNames();

  man.AddTask(var_manager);

  man.Init();
  man.Run(-1);
  man.Finish();

}

}



#endif //ANALYSISTREE_INFRA_VARMANAGER_TEST_HPP_
