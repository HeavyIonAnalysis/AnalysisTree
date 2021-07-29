/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "Variable.hpp"

#include <algorithm>
#include <regex>

#include "Configuration.hpp"

#include <iostream>

namespace AnalysisTree {

Variable::Variable(std::string name, std::vector<Field> fields, std::function<double(std::vector<double>&)> lambda)
    : name_(std::move(name)),
      fields_(std::move(fields)),
      lambda_(std::move(lambda)),
      n_branches_(GetBranches().size())
{
}

void Variable::Init(const Configuration& conf) {
  for (auto& field : fields_) {
    field.Init(conf);
  }
  vars_.reserve(fields_.size());
  is_init_ = true;
}

void Variable::Print() const {
  std::cout << "Variable::Print():" << std::endl;
  std::cout << "  name: " << name_ << std::endl;
  for (const auto& field : fields_) {
    field.Print();
  }
}

bool operator==(const AnalysisTree::Variable& that, const AnalysisTree::Variable& other) {
  if (&that == &other) {
    return true;
  }
  return that.name_ == other.name_ && that.fields_ == other.fields_;
}

bool operator>(const AnalysisTree::Variable& that, const AnalysisTree::Variable& other) {
  return that.name_ > other.name_;
}

bool operator<(const AnalysisTree::Variable& that, const AnalysisTree::Variable& other) {
  return that.name_ > other.name_;
}

Variable Variable::FromString(const std::string& full_name) {
  const std::regex name_regex("^(\\w+)[\\.\\/](\\w+)$");
  std::smatch match;
  auto search_ok = std::regex_search(full_name, match, name_regex);
  if (search_ok) {
    std::string branch_name{match.str(1)};
    std::string field_name{match.str(2)};
    return Variable(branch_name, field_name);
  }
  throw std::runtime_error("Field name must be in the format <branch>.<name>");
}

double Variable::GetValue(const BranchChannel& object) const {
  assert(is_init_ && n_branches_ == 1);
  vars_.clear();
  for (const auto& field : fields_) {
    vars_.emplace_back(object[field]);
  }
  return lambda_(vars_);
}

double Variable::GetValue(const BranchChannel& a, size_t a_id, const BranchChannel& b, size_t b_id) const {
  assert(is_init_);
  vars_.clear();
  for (const auto& field : fields_) {
    if (field.GetBranchId() == a_id)
      vars_.emplace_back(a[field]);
    else if (field.GetBranchId() == b_id)
      vars_.emplace_back(b[field]);
    else {
      throw std::runtime_error("Variable::Fill - Cannot fill value from branch " + field.GetBranchName());
    }
  }
  return lambda_(vars_);
}
std::string Variable::GetBranchName() const {
  if(n_branches_ != 1){
    throw std::runtime_error("Number of branches is not 1!");
  }
  return fields_.at(0).GetBranchName();
}

std::set<std::string> Variable::GetBranches() const {
  std::set<std::string> branches{};
  for(const auto& field : fields_){
    branches.emplace(field.GetBranchName());
  }
  return branches;
}

}// namespace AnalysisTree
