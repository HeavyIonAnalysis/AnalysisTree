/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_INFRA_BRANCH_TEST_CPP_
#define ANALYSISTREE_INFRA_BRANCH_TEST_CPP_

#include <gtest/gtest.h>

#include "TaskManager.hpp"
#include "ToyMC.hpp"

#include "Branch.hpp"

#include <random>

namespace {

using namespace AnalysisTree;

class WriteTask : public Task {

 public:
  WriteTask() = default;

  void Init() override {
    auto man = TaskManager::GetInstance();
    auto particles = man->GetChain()->GetBranch("SimParticles");
    auto new_conf = particles.GetConfig().Clone("NewParticles", DetType::kParticle);

    man->AddBranch(ptr, new_conf);
    auto new_particles = Branch(new_conf, ptr);
  }
  void Exec() override {
//    ptr = std::get<Particles*>(particles_.GetData());
  }
  void Finish() override {

  }

 private:
  Branch* particles_;
  Branch* new_particles_;

  Particles* ptr{nullptr};

};

TEST(Branch, Write) {
//  const int n_events = 1000;
//  const std::string filelist = "fl_toy_mc.txt";
//
//  RunToyMC(n_events, filelist);
//
//  auto* man = TaskManager::GetInstance();
//  auto* task = new WriteTask();
//
//  man->AddTask(task);
//
//  man->Init({filelist}, {"tTree"});
//  man->Run(-1);
//  man->Finish();
}


TEST(Branch, Basics) {

  RunToyMC(1000);
  Chain t("toymc_analysis_task.root", "tTree");
  t.InitPointersToBranches({"SimParticles"});

  auto branch = t.GetBranch("SimParticles");
  Field pT = branch.GetField("pT");

//  Particles* ptr{nullptr};
//  BranchConfig test("test", DetType::kParticle);
//  auto test_br = t.AddBranch(ptr, test);

  for(int i=0; i<1; ++i){
    t.GetEntry(i);

    for(int j=0; j<branch.size(); ++j){
      auto ch = branch[j];


      std::cout << ch[pT] << std::endl;
    }
  }
}

}// namespace


#endif//ANALYSISTREE_INFRA_BRANCH_TEST_CPP_
