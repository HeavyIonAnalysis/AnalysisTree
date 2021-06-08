#include "VarManager.hpp"
#include <numeric>

namespace AnalysisTree {

void VarManager::Init(std::map<std::string, void*>& pointers_map) {

  branches_.reserve(in_branches_.size());
  for (const auto& branch : in_branches_) {
    const auto type = config_->GetBranchConfig(branch).GetType();
    auto* ptr = pointers_map.find(branch)->second;
    auto find = cuts_map_.find(branch);
    Cuts* cut = find != cuts_map_.end() ? find->second : nullptr;
    if (cut) {
      cut->Init(*config_);
    }
    branches_.emplace_back(BranchReader(branch, ptr, type, cut));
  }

  for (auto& var : entries_) {
    var.Init(*config_, pointers_map);
    for (const auto& br : var.GetBranchNames()) {
      var.AddBranchPointer(GetBranch(br));
    }
  }// vars
}

void VarManager::Exec() {
  for (auto& var : entries_) {
    var.FillValues();
  }
}

void VarManager::FillBranchNames() {
  for (auto& var : entries_) {
    const auto& br = var.GetBranchNames();
    in_branches_.insert(in_branches_.end(), br.begin(), br.end());
  }
  std::sort(in_branches_.begin(), in_branches_.end());
  auto ip = std::unique(in_branches_.begin(), in_branches_.end());
  in_branches_.resize(std::distance(in_branches_.begin(), ip));
}

std::pair<int, std::vector<int>> VarManager::AddEntry(const VarManagerEntry& vars) {

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
  std::iota(var_ids.begin(), var_ids.end(), 0);// var_ids will become: [0..size]
  return std::make_pair(entries_.size() - 1, var_ids);
}

BranchReader* VarManager::GetBranch(const std::string& name) {
  for (auto& branch : branches_) {
    if (branch.GetName() == name) {
      return &branch;
    }
  }
  throw std::runtime_error("Branch " + name + " is not found");
}

}// namespace AnalysisTree