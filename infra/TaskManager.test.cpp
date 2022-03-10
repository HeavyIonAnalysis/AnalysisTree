/* Copyright (C) 2022, Universität Tübingen
SPDX-License-Identifier: GPL-3.0-only
Authors: Viktor Klochkov */
#ifndef ANALYSISTREE_INFRA_TASKMANANGER_TEST_HPP_
#define ANALYSISTREE_INFRA_TASKMANANGER_TEST_HPP_

#include <gtest/gtest.h>
#include "TaskManager.hpp"
#include "ToyMC.hpp"

namespace {

using namespace AnalysisTree;

TEST(TaskManager, RemoveBranch) {

  const int n_events = 1000;
  const std::string filelist = "fl_test_task_manager.txt";

  RunToyMC(n_events, filelist);

  TaskManager* man = TaskManager::GetInstance();
  man->SetWriteMode(eBranchWriteMode::kCopyTree);
  man->SetBranchesExclude({"SimParticles"});
  man->SetOutputName("test.root", "tTree");

  man->Init({filelist}, {"tTree"});
  man->Run(-1);
  man->Finish();

  Chain t1(std::vector<std::string>{filelist}, {"tTree"});
  Chain t2("test.root", "tTree");

  ASSERT_GT(t1.GetListOfBranches()->GetEntries(), t2.GetListOfBranches()->GetEntries());
  ASSERT_GT(t1.GetFile()->GetSize(), t2.GetFile()->GetSize());
}


}

#endif//ANALYSISTREE_INFRA_TASKMANANGER_TEST_HPP_
