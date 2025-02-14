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

bool Cuts::Apply(std::vector<const BranchChannel*>& bch, std::vector<size_t>& id) const {
  if (!is_init_) {
    throw std::runtime_error("Cuts::Apply - cut is not initialized!!");
  }
  if (bch.size() != id.size()) {
    throw std::runtime_error("AnalysisTree::Cuts::Apply() - BranchChannel and Id vectors must have the same size");
  }
  //    std::all_of(cuts_.begin(), cuts_.end(), Apply(a, a_id, b, b_id)); //TODO
  for (const auto& cut : cuts_) {
    if (!cut.Apply(bch, id))
      return false;
  }
  return true;
}

bool Cuts::Apply(const BranchChannel& a, size_t a_id, const BranchChannel& b, size_t b_id) const {
  BranchChannel* a_ptr = new BranchChannel(std::move(a));
  BranchChannel* b_ptr = new BranchChannel(std::move(b));
  std::vector<const BranchChannel*> brch_vec{a_ptr, b_ptr};
  std::vector<size_t> id_vec{a_id, b_id};
  bool result = Apply(brch_vec, id_vec);
  delete a_ptr;
  delete b_ptr;
  return result;
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

void Cuts::AddCut(const SimpleCut& cut) {
  cuts_.emplace_back(cut);
  branch_names_.insert(cut.GetBranches().begin(), cut.GetBranches().end());
}

void Cuts::AddCuts(const std::vector<SimpleCut>& cuts) {
  for (auto& cut : cuts) {
    AddCut(cut);
  }
}

}// namespace AnalysisTree
