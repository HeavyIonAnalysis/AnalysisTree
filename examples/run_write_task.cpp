/* Copyright (C) 2025 GSI, Heidelberg University
SPDX-License-Identifier: GPL-3.0-only
Authors: Oleksii Lubynets, Ilya Selyuzhenkov */
#include "UserTaskWrite.hpp"
#include <TaskManager.hpp>

using namespace AnalysisTree;

void run_write_task(const std::string& filelist, const std::string& treename);

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Error! Please use " << std::endl;
    std::cout << " ./run_write_task filelist treename" << std::endl;
    exit(EXIT_FAILURE);
  }

  const std::string filelist = argv[1];
  const std::string treename = argv[2];
  run_write_task(filelist, treename);

  return 0;
}

void run_write_task(const std::string& filelist, const std::string& treename) {
  auto* man = TaskManager::GetInstance();
  man->SetOutputName("UTW_output.root", "aTree");
  man->SetWriteMode(eBranchWriteMode::kCopyTree);

  auto* task = new UserTaskWrite();
  man->AddTask(task);

  man->Init({filelist}, {treename});
  man->Run(2);
  man->Finish();
}