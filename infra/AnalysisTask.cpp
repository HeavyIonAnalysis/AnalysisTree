#include "AnalysisTask.hpp"
#include "TaskManager.hpp"

#include <numeric>

namespace AnalysisTree {

void AnalysisTask::Init() {
  assert(!is_init_);

  auto* chain = TaskManager::GetInstance()->GetChain();

  branches_.reserve(in_branches_.size());
  for (const auto& branch : in_branches_) {
    const auto type = config_->GetBranchConfig(branch).GetType();
    BranchPointer ptr = chain->GetPointerToBranch(branch);
    auto find = cuts_map_.find(branch);
    Cuts* cut = find != cuts_map_.end() ? find->second : nullptr;
    if (cut) {
      cut->Init(*config_);
    }
    branches_.emplace_back(BranchReader(branch, ptr, type, cut));
  }

  for (auto& var : entries_) {
    var.Init(*config_, chain->GetMatchPointers());
    for (const auto& br : var.GetBranchNames()) {
      var.AddBranchPointer(GetBranch(br));
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

    if (vars.GetBranchNames() == entries_[ivar].GetBranchNames() && Cuts::Equal(vars.GetCuts(), entries_[ivar].GetCuts())) { //branch exists
      for (size_t i = 0; i < vars.GetVariables().size(); ++i) {
        var_ids[i] = entries_[ivar].AddVariable(vars.GetVariables()[i]);
      }
      return std::make_pair(ivar, var_ids);
    }
  }
  entries_.emplace_back(vars);
  std::iota(var_ids.begin(), var_ids.end(), 0); // var_ids will become: [0..size-1]
  return std::make_pair(entries_.size() - 1, var_ids);
}

BranchReader AnalysisTask::GetBranch(const std::string& name) {
  for (auto& branch : branches_) {
    if (branch.GetName() == name) {
      return branch;
    }
  }
  throw std::runtime_error("Branch " + name + " is not found");
}

}// namespace AnalysisTree