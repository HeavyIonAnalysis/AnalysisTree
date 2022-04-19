/* Copyright (C) 2022, Universität Tübingen
SPDX-License-Identifier: GPL-3.0-only
Authors: Viktor Klochkov */
#ifndef ANALYSISTREE_INFRA_TASKMANANGER_TEST_HPP_
#define ANALYSISTREE_INFRA_TASKMANANGER_TEST_HPP_

#include "TaskManager.hpp"
#include "ToyMC.hpp"
#include <gtest/gtest.h>

namespace {

using namespace AnalysisTree;

class TestTask : public Task {
 public:
  void Init() override {
    auto man = TaskManager::GetInstance();
    auto chain = man->GetChain();

    rec_ = chain->GetBranchObject("RecTracks");
    sim_ = chain->GetBranchObject("SimParticles");
  }

  void Exec() override {
    // just do something to check data
    auto r = rec_.size();
    auto s = sim_.size();
    if (r != s) {
      std::cout << r << " " << s << std::endl;
    }
  }

  void Finish() override {}

 protected:
  Branch rec_;
  Branch sim_;
};

TEST(TaskManager, RemoveBranch) {

  const int n_events = 1000;
  const std::string filelist = "fl_test_task_manager.txt";

  RunToyMC(n_events, filelist);

  TaskManager* man = TaskManager::GetInstance();
  man->SetWriteMode(eBranchWriteMode::kCopyTree);
  man->SetBranchesExclude({"SimParticles"});
  man->SetOutputName("test.root", "tTree");
  man->AddTask(new TestTask);

  man->Init({filelist}, {"tTree"});
  man->Run(-1);
  man->Finish();

  Chain t1(std::vector<std::string>{filelist}, {"tTree"});
  Chain t2("test.root", "tTree");

  ASSERT_GT(t1.GetListOfBranches()->GetEntries(), t2.GetListOfBranches()->GetEntries());
  ASSERT_GT(t1.GetFile()->GetSize(), t2.GetFile()->GetSize());
  ASSERT_EQ(t1.GetEntries(), t2.GetEntries());

  t1.InitPointersToBranches({});
  t2.InitPointersToBranches({});

  auto br1 = t1.GetBranchObject("RecTracks");
  auto br2 = t2.GetBranchObject("RecTracks");

  t1.GetEntry(0);
  t2.GetEntry(0);

  ASSERT_EQ(br1.size(), br1.size());
  for (size_t i = 0; i < br1.size(); ++i) {
    auto track1 = *(br1[i].Data<Track>());
    auto track2 = *(br2[i].Data<Track>());
    ASSERT_EQ(track1, track2);
  }
}

}// namespace

#endif//ANALYSISTREE_INFRA_TASKMANANGER_TEST_HPP_
