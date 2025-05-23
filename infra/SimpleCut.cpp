/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "SimpleCut.hpp"

#include "HelperFunctions.hpp"

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
  // if both SimpleCuts were defined via lambda, they're assumed not equal (unless they're in the same memory place)
  if (that.hash_ == 1 && other.hash_ == 1) return false;
  return that.vars_ == other.vars_ && that.title_ == other.title_ && that.hash_ == other.hash_;
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

SimpleCut OpenCut(const std::string& branchName, const std::string& title) {
  return SimpleCut({branchName + ".ones"}, [](const std::vector<double>& par) { return true; });
}

SimpleCut::SimpleCut(const Variable& var, int value, std::string title) : title_(std::move(title)) {
  vars_.emplace_back(var);
  lambda_ = [value](std::vector<double>& vars) { return vars[0] <= value + SmallNumber && vars[0] >= value - SmallNumber; };
  FillBranchNames();
  const std::string stringForHash = var.GetName() + HelperFunctions::ToStringWithPrecision(value, 6) + title_;
  std::hash<std::string> hasher;
  hash_ = hasher(stringForHash);
}

SimpleCut::SimpleCut(const Variable& var, double min, double max, std::string title) : title_(std::move(title)) {
  vars_.emplace_back(var);
  lambda_ = [max, min](std::vector<double>& vars) { return vars[0] <= max && vars[0] >= min; };
  FillBranchNames();
  const std::string stringForHash = var.GetName() + HelperFunctions::ToStringWithPrecision(min, 6) + HelperFunctions::ToStringWithPrecision(max, 6) + title_;
  std::hash<std::string> hasher;
  hash_ = hasher(stringForHash);
}

bool SimpleCut::Apply(std::vector<const BranchChannel*>& bch, std::vector<size_t>& id) const {
  if (bch.size() != id.size()) {
    throw std::runtime_error("AnalysisTree::SimpleCut::Apply() - BranchChannel and Id vectors must have the same size");
  }
  std::vector<double> variables;
  variables.reserve(vars_.size());
  for (const auto& var : vars_) {
    variables.emplace_back(var.GetValue(bch, id));
  }
  return lambda_(variables);
}

bool SimpleCut::Apply(const BranchChannel& a, size_t a_id, const BranchChannel& b, size_t b_id) const {
  BranchChannel* a_ptr = new BranchChannel(std::move(a));
  BranchChannel* b_ptr = new BranchChannel(std::move(b));
  std::vector<const BranchChannel*> brch_vec{a_ptr, b_ptr};
  std::vector<size_t> id_vec{a_id, b_id};
  bool result = Apply(brch_vec, id_vec);
  delete a_ptr;
  delete b_ptr;
  return result;
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
