#include "VarManager.hpp"

namespace AnalysisTree {

void VarManager::Init(std::map<std::string, void *> &pointers_map) {
  assert(!in_branches_.empty());

  for (auto &var : vars_) {
    var.Init(*config_);
  }

  for (const auto &branch : in_branches_) {
    const auto type = config_->GetBranchConfig(branch).GetType();
    auto *ptr = pointers_map.find(branch)->second;
    auto find = cuts_map_.find(branch);
    Cuts *cut = find != cuts_map_.end() ? find->second : nullptr;
    if (cut) {
      cut->Init(*config_);
    }
    branches_.emplace_back(BranchReader(branch, ptr, type, cut));

    for (const auto &var : vars_) {
      if (var.GetBranchName() == branch) {
        branches_.back().AddVariable(var);
      }
    }
  }
}

void VarManager::Exec() {
  for (auto &branch : branches_) {
    branch.FillValues();
  }
}

void VarManager::FillOutputIndexes(const std::vector<Variable> &vars) {
  var_indexes_.reserve(vars.size());

  std::vector<int> branch_entries(in_branches_.size(), 0);
  std::vector<std::pair<int, int>> unique_var_indexes{};
  for (const auto &var : vars_) {
    unique_var_indexes.emplace_back(std::make_pair(-1, -1));

    for (size_t i_branch = 0; i_branch < in_branches_.size(); ++i_branch) {
      if (var.GetBranchName() == in_branches_.at(i_branch)) {
        unique_var_indexes.back().first = i_branch;
        unique_var_indexes.back().second = branch_entries.at(i_branch)++;
      }
    }
  }

  for (size_t i_var = 0; i_var < vars.size(); ++i_var) {
    var_indexes_.emplace_back(unique_var_indexes.at(unique_map_.find(i_var)->second));
  }
}

void VarManager::CopyUniqueVars(const std::vector<Variable> &vars) {
  vars_.reserve(vars.size());
  for (const auto &var : vars) {
    vars_.emplace_back(var);
  }

  std::sort(vars_.begin(), vars_.end());
  auto ip = std::unique(vars_.begin(), vars_.end());
  vars_.resize(std::distance(vars_.begin(), ip));

  CreateMapUnique(vars);
}

void VarManager::CreateMapUnique(const std::vector<Variable> &vars) {
  int i_var{0};
  for (const auto &var : vars) {
    int i_var_unique{0};
    for (const auto &var_unique : vars_) {
      if (var_unique == var) {
        unique_map_.insert(std::make_pair(i_var, i_var_unique));
        break;
      }
      i_var_unique++;
    }
    i_var++;
  }
}

void VarManager::FillBranchNames() {
  for (const auto &var : vars_) {
    for (const auto &branch : var.GetBranches())
      in_branches_.emplace_back(branch);
  }
  std::sort(in_branches_.begin(), in_branches_.end());
  auto ip1 = std::unique(in_branches_.begin(), in_branches_.end());
  in_branches_.resize(std::distance(in_branches_.begin(), ip1));
}
}// namespace AnalysisTree