/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include <iostream>

#include <AnalysisTree/ToyMC.hpp>
#include <AnalysisTree/Branch.hpp>

#include "UserTaskRead.hpp"
#include "UserTaskWrite.hpp"

using namespace AnalysisTree;

int main(int argc, char* argv[]) {

  const int n_events = 10;
  const std::string filelist = "fl_toy_mc.txt";

  RunToyMC(n_events, filelist);

  auto* man = TaskManager::GetInstance();
  man->SetOutputMode(eBranchWriteMode::kCopyTree);

  auto* task_read = new UserTaskRead();
  auto* task_write = new UserTaskWrite();

  man->AddTask(task_read);
  man->AddTask(task_write);

  man->Init({filelist}, {"tTree"});
  man->Run(-1);
  man->Finish();
}
