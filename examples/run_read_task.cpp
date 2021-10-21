/* Copyright (C) 2019-2021 GSI, Universität Tübingen
SPDX-License-Identifier: GPL-3.0-only
Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "UserTaskRead.hpp"
#include <TaskManager.hpp>
#include <iostream>

using namespace AnalysisTree;

void run_read_task(const std::string& filelist, const std::string& treename);

int main(int argc, char* argv[]) {

  if (argc < 3) {
    std::cout << "Error! Please use " << std::endl;
    std::cout << " ./example filelist treename" << std::endl;
    exit(EXIT_FAILURE);
  }

  const std::string filelist = argv[1];
  const std::string treename = argv[2];
  run_read_task(filelist, treename);

  return 0;
}

void run_read_task(const std::string& filelist, const std::string& treename){

  auto* man = TaskManager::GetInstance();
  auto* task = new UserTaskRead();
  man->AddTask(task);

  man->Init({filelist}, {treename});
  man->Run(2);
  man->Finish();
}
