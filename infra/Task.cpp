/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "Task.hpp"
#include "TaskManager.hpp"

namespace AnalysisTree {

void Task::PreInit() {
  const auto* const man = TaskManager::GetInstance();
  config_ = man->GetConfig();
  data_header_ = man->GetDataHeader();

  if (event_cuts_) {
    event_cuts_->Init(*config_);
  }
}

}// namespace AnalysisTree