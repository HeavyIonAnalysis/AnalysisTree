/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "AnalysisTask.hpp"
#include "TaskManager.hpp"

#include <numeric>

namespace AnalysisTree {

void AnalysisTask::Init() {
  assert(!is_init_);
  auto* chain = TaskManager::GetInstance()->GetChain();

  for (auto& var : entries_) {
    var.Init(*config_, chain->GetMatchPointers());
    for (const auto& br : var.GetBranchNames()) {
      var.AddBranch(chain->GetBranch(br));
    }
  }// vars
  is_init_ = true;
}

void AnalysisTask::Exec() {
  for (auto& var : entries_) {
    var.FillValues();
  }
}

std::pair<int, std::vector<int>> AnalysisTask::AddEntry(const AnalysisEntry& vars) {
  in_branches_.insert(vars.GetBranchNames().begin(), vars.GetBranchNames().end());
  std::vector<int> var_ids(vars.GetVariables().size());

  for (size_t ivar = 0; ivar < entries_.size(); ++ivar) {
    if (vars.GetBranchNames() == entries_[ivar].GetBranchNames() && Cuts::Equal(vars.GetCuts(), entries_[ivar].GetCuts())) {//branch exists
      for (size_t i = 0; i < vars.GetVariables().size(); ++i) {
        var_ids[i] = entries_[ivar].AddVariable(vars.GetVariables()[i]);
      }
      return std::make_pair(ivar, var_ids);
    }
  }
  entries_.emplace_back(vars);
  std::iota(var_ids.begin(), var_ids.end(), 0);// var_ids will become: [0..size-1]
  return std::make_pair(entries_.size() - 1, var_ids);
}

}// namespace AnalysisTree