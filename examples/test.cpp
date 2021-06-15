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

  auto config = t.GetConfiguration();

  const auto& ptrs = t.GetBranchPointers();

  auto particles_var = ptrs.find("SimParticles")->second;
  auto particles = std::get<Particles*>(particles_var);

  Branch branch(config->GetBranchConfig("SimParticles"), particles);
  Field pT("SimParticles", "pT");
  pT.Init(*config);

  for(int i=0; i<1; ++i){
    t.GetEntry(i);

    for(int j=0; j<branch.size(); ++j){
      auto ch = branch[j];
      std::cout << ch[pT] << std::endl;
      ch.Print();
    }

//    for(const auto& channel : branch){
//
//    }
  }


}
