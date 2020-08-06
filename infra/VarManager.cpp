#include "VarManager.hpp"

namespace AnalysisTree {

void VarManager::Init(std::map<std::string, void *> &pointers_map) {

  branches_.reserve(in_branches_.size());
  for (const auto &branch : in_branches_) {
    const auto type = config_->GetBranchConfig(branch).GetType();
    auto *ptr = pointers_map.find(branch)->second;
    auto find = cuts_map_.find(branch);
    Cuts *cut = find != cuts_map_.end() ? find->second : nullptr;
    if (cut) {
      cut->Init(*config_);
    }
    branches_.emplace_back(BranchReader(branch, ptr, type, cut));
  }

  for (auto &var : vars_) {
    var.Init(*config_, pointers_map);
    for (const auto& br : var.GetBranches()) {
      var.AddBranchPointer(GetBranch(br));
    }
  } // vars
}

void VarManager::Exec() {
  for (auto &var : vars_) {
    var.FillValues();
  }
}

BranchReader* VarManager::GetBranch(const std::string& name) {
  for (auto &branch : branches_) {
    if (branch.GetName() == name) {
      return &branch;
    }
  }
  throw std::runtime_error("Branch " + name + " is not found");
}

void VarManager::FillBranchNames() {
  for (auto &var : vars_) {
    var.FillBranchNames();
    const auto& br = var.GetBranches();
    in_branches_.insert(in_branches_.end(), br.begin(), br.end());
  }
  std::sort(in_branches_.begin(), in_branches_.end());
  auto ip = std::unique(in_branches_.begin(), in_branches_.end());
  in_branches_.resize(std::distance(in_branches_.begin(), ip));
}

size_t VarManager::AddEntry(const VarManagerEntry& vars) {
  vars_.emplace_back(vars);
  return vars_.size()-1;
}

}// namespace AnalysisTree