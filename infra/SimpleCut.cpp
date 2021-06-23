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

SimpleCut RangeCut(const std::string& variable_name, float lo, float hi, const std::string& title) {
  return SimpleCut(Variable::FromString(variable_name), lo, hi, title);
}

SimpleCut EqualsCut(const std::string& variable_name, int value, const std::string& title) {
  return SimpleCut(Variable::FromString(variable_name), value, title);
}

//SimpleCut RangeCut(const std::string& variable_name, float lo, float hi, const std::string& title) {
//  return SimpleCut(Variable::FromString(variable_name), lo, hi, title);
//}
//SimpleCut EqualsCut(const std::string& variable_name, float value, const std::string& title) {
//  return SimpleCut(Variable::FromString(variable_name), value, title);
//}
}// namespace AnalysisTree

ClassImp(AnalysisTree::SimpleCut)
