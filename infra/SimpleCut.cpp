/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "SimpleCut.hpp"

#include <iostream>

namespace AnalysisTree {

void SimpleCut::Print() const {
  std::cout << title_ << std::endl;
}

void SimpleCut::FillBranchNames() {
  for (const auto& v : vars_) {
    const auto& br = v.GetBranches();
    branch_names_.insert(br.begin(), br.end());
  }
}

bool operator==(const SimpleCut& that, const SimpleCut& other) {
  if (&that == &other) {
    return true;
  }
  return that.vars_ == other.vars_ && that.title_ == other.title_;
}

SimpleCut RangeCut(const std::string& variable_name, double lo, double hi, const std::string& title) {
  return SimpleCut(Variable::FromString(variable_name), lo, hi, title);
}

SimpleCut EqualsCut(const std::string& variable_name, int value, const std::string& title) {
  return SimpleCut(Variable::FromString(variable_name), value, title);
}

SimpleCut RangeCut(const Variable& var, double lo, double hi, const std::string& title) {
  return SimpleCut(var, lo, hi, title);
}

SimpleCut EqualsCut(const Variable& var, int value, const std::string& title) {
  return SimpleCut(var, value, title);
}

SimpleCut::SimpleCut(const Variable& var, int value, std::string title) : title_(std::move(title)) {
  vars_.emplace_back(var);
  lambda_ = [value](std::vector<double>& vars) { return vars[0] <= value + SmallNumber && vars[0] >= value - SmallNumber; };
  FillBranchNames();
}

SimpleCut::SimpleCut(const Variable& var, double min, double max, std::string title) : title_(std::move(title)) {
  vars_.emplace_back(var);
  lambda_ = [max, min](std::vector<double>& vars) { return vars[0] <= max && vars[0] >= min; };
  FillBranchNames();
}

bool SimpleCut::Apply(const BranchChannel& a, size_t a_id, const BranchChannel& b, size_t b_id) const {
  std::vector<double> variables;
  variables.reserve(vars_.size());
  for (const auto& var : vars_) {
    variables.emplace_back(var.GetValue(a, a_id, b, b_id));
  }
  return lambda_(variables);
}

bool SimpleCut::Apply(const BranchChannel& object) const {
  std::vector<double> variables;
  variables.reserve(vars_.size());
  for (const auto& var : vars_) {
    variables.emplace_back(var.GetValue(object));
  }
  return lambda_(variables);
}
}// namespace AnalysisTree

ClassImp(AnalysisTree::SimpleCut)
