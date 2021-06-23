/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include <iostream>
#include <random>

#include <AnalysisTree/ToyMC.hpp>
#include <AnalysisTree/Branch.hpp>

using namespace AnalysisTree;

int main(int argc, char* argv[]) {

  RunToyMC(1000);
  Chain t("toymc_analysis_task.root", "tTree");
  t.InitPointersToBranches({"SimParticles"});

  auto branch = t.GetBranch("SimParticles");
  Field pT = branch.GetField("pT");

  Particles* ptr{nullptr};
  BranchConfig test("test", DetType::kParticle);
  auto test_br = t.AddBranch(ptr, test);

  for(int i=0; i<1; ++i){
    t.GetEntry(i);

    for(int j=0; j<branch.size(); ++j){
      auto ch = branch[j];
      std::cout << ch[pT] << std::endl;
    }
  }

}
