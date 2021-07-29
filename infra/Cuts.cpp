/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "Cuts.hpp"
#include "Configuration.hpp"

#include <iostream>

namespace AnalysisTree {

void Cuts::Init(const AnalysisTree::Configuration& conf) {
  for (auto& cut : cuts_) {
    for (auto& var : cut.Variables()) {
      var.Init(conf);
    }
  }
  for (const auto& branch : branch_names_) {
    branch_ids_.insert(conf.GetBranchConfig(branch).GetId());
  }
  is_init_ = true;
}

void Cuts::Print() const {
  std::cout << "Cut " << name_ << " defined as:" << std::endl;
  for (const auto& cut : cuts_) {
    cut.Print();
  }
}

bool operator==(const Cuts& that, const Cuts& other) {
  if (&that == &other) {
    return true;
  }
  return that.name_ == other.name_ && that.cuts_ == other.cuts_;
}

bool Cuts::Equal(const Cuts* that, const Cuts* other) {
  if (that == nullptr && other == nullptr) { return true; }
  if (that && other) { return *that == *other; }
  return false;
}

bool Cuts::Apply(const BranchChannel& a, size_t a_id, const BranchChannel& b, size_t b_id) const {
  if (!is_init_) {
    throw std::runtime_error("Cuts::Apply - cut is not initialized!!");
  }
  //    std::all_of(cuts_.begin(), cuts_.end(), Apply(a, a_id, b, b_id)); //TODO
  for (const auto& cut : cuts_) {
    if (!cut.Apply(a, a_id, b, b_id))
      return false;
  }
  return true;
}

bool Cuts::Apply(const BranchChannel& ob) const {
  if (!is_init_) {
    throw std::runtime_error("Cuts::Apply - cut is not initialized!!");
  }
  for (const auto& cut : cuts_) {
    if (!cut.Apply(ob))
      return false;
  }
  return true;
}

}// namespace AnalysisTree