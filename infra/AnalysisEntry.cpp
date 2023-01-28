/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "AnalysisEntry.hpp"

#include "Configuration.hpp"
#include "VariantMagic.hpp"

namespace AnalysisTree {

AnalysisEntry::~AnalysisEntry() {
  for(auto& br : branches_) {
    delete br.first;
  }
}

void AnalysisEntry::AddBranch(const Branch& branch, Cuts* cuts) {
  branches_.emplace_back(std::make_pair(new Branch(branch), cuts));
}

bool AnalysisEntry::ApplyCutOnBranch(const Branch& br, Cuts* cuts, int i_channel) const {
  if (cuts && !cuts->Apply(br[i_channel])) { return false; }
  return !cuts_ || cuts_->Apply(br[i_channel]);
}

bool AnalysisEntry::ApplyCutOnBranches(std::vector<const Branch*>& br, std::vector<Cuts*>& cuts, std::vector<int>& ch) const {
  if(br.size() != cuts.size() || cuts.size() != ch.size()) {
    throw std::runtime_error("AnalysisTree::AnalysisEntry::ApplyCutOnBranches() - Branch, Cuts and Ch vectors must have the same size");
  }
  std::vector<const BranchChannel*> bch_vec;
  std::vector<size_t> id_vec;
  bch_vec.reserve(br.size());
  id_vec.reserve(br.size());
  for(int i=0; i<br.size(); i++) {
    BranchChannel* bchptr = new BranchChannel(br.at(i), ch.at(i));
    bch_vec.emplace_back(bchptr);
    id_vec.emplace_back(br.at(i)->GetId());
  }
  bool result = !cuts_ || cuts_->Apply(bch_vec, id_vec);
  for(auto& bv : bch_vec) {
    delete bv;
  }
  return result;
}

bool AnalysisEntry::ApplyCutOnBranches(const Branch& br1, Cuts* cuts1, int ch1, const Branch& br2, Cuts* cuts2, int ch2) const {
  Branch* br1_ptr = new Branch(std::move(br1));
  Branch* br2_ptr = new Branch(std::move(br2));
  std::vector<const Branch*> br_vec{br1_ptr, br2_ptr};
  std::vector<Cuts*> cuts_vec{cuts1, cuts2};
  std::vector<int> ch_vec{ch1, ch2};
  bool result = ApplyCutOnBranches(br_vec, cuts_vec, ch_vec);
  delete br1_ptr;
  delete br2_ptr;
  return result;
}

double AnalysisEntry::FillVariable(const Variable& var, std::vector<const Branch*>& br, std::vector<int>& id) {
  if(br.size() != id.size()) {
    throw std::runtime_error("AnalysisTree::AnalysisEntry::FillVariable() - Branch and Id vectors must have the same size");
  }
  std::vector<const BranchChannel*> bch_vec;
  std::vector<size_t> id_vec;
  bch_vec.reserve(br.size());
  id_vec.reserve(br.size());
  for(int i=0; i<br.size(); i++) {
    BranchChannel* bchptr = new BranchChannel(br.at(i), id.at(i));
    bch_vec.emplace_back(bchptr);
    id_vec.emplace_back(br.at(i)->GetId());
  }
  double result = var.GetValue(bch_vec, id_vec);
  for(auto& bv : bch_vec) {
    delete bv;
  }
  return result;
}

double AnalysisEntry::FillVariable(const Variable& var, const Branch& br1, int ch1, const Branch& br2, int ch2) {
  Branch* br1_ptr = new Branch(std::move(br1));
  Branch* br2_ptr = new Branch(std::move(br2));
  std::vector<const Branch*> br_vec{br1_ptr, br2_ptr};
  std::vector<int> ch_vec{ch1, ch2};
  double result = FillVariable(var, br_vec, ch_vec);
  delete br1_ptr;
  delete br2_ptr;
  return result;
}

void AnalysisEntry::FillValues() {
  values_.clear();
  if (non_eve_header_indices_.size() == 0) {
    FillFromEveHeaders();
  } else if (non_eve_header_indices_.size() == 1) {
    FillFromOneChannalizedBranch();
  } else if (non_eve_header_indices_.size() == 2) {
    FillFromTwoChannalizedBranches();
  }
}

/**
* @brief FillFromEveHeaders populates Variables from event headers only
*/
void AnalysisEntry::FillFromEveHeaders() {
  std::vector<const Branch*> br_vec;
  std::vector<Cuts*> cuts_vec;
  std::vector<int> id_vec;
  br_vec.reserve(branches_.size());
  cuts_vec.reserve(branches_.size());
  id_vec.reserve(branches_.size());
  for(auto& br : branches_) {
    br_vec.emplace_back(br.first);
    cuts_vec.emplace_back(br.second);
    id_vec.emplace_back(0);
  }

  values_.reserve(1);
  if (!ApplyCutOnBranches(br_vec, cuts_vec, id_vec)) return;
  std::vector<double> temp_vars(vars_.size());
  short i_var{0};
  for (const auto& var : vars_) {
    temp_vars[i_var] = FillVariable(var, br_vec, id_vec);
    i_var++;
  }//variables
  values_.emplace_back(temp_vars);
}

/**
* @brief FillFromOneChannalizedBranch populates Variables from one channalized branch
* and any number of event headers.
*/
void AnalysisEntry::FillFromOneChannalizedBranch() {
  const auto n_channels = branches_.at(non_eve_header_indices_.at(0)).first->size();
  values_.reserve(n_channels);

  std::vector<const Branch*> br_vec;
  std::vector<Cuts*> cuts_vec;
  std::vector<int> id_vec;
  br_vec.reserve(branches_.size());
  cuts_vec.reserve(branches_.size());
  id_vec.resize(branches_.size());
  for(const auto& br : branches_) {
    br_vec.emplace_back(br.first);
    cuts_vec.emplace_back(br.second);
  }
  for(auto& ehi : eve_header_indices_) {
    id_vec.at(ehi) = 0;
  }

  for (size_t i_channel = 0; i_channel < n_channels; ++i_channel) {
    id_vec.at(non_eve_header_indices_.at(0)) = i_channel;
    if (!ApplyCutOnBranches(br_vec, cuts_vec, id_vec)) continue;
    std::vector<double> temp_vars(vars_.size());
    short i_var{0};
    for (const auto& var : vars_) {
      temp_vars[i_var] = FillVariable(var, br_vec, id_vec);
      i_var++;
    }//variables
    values_.emplace_back(temp_vars);
  }// channels
}

/**
* @brief FillFromTwoChannalizedBranches populates Variables from two channalized branches
* and any number of event headers.
* It iterates over registered matches and fills variables
*/
void AnalysisEntry::FillFromTwoChannalizedBranches() {
  if(matching_ == nullptr) {
    throw std::runtime_error("AnalysisEntry::FillFromTwoChannalizedBranches() - Matching between non-EventHeader branches must be set");
  }

  values_.reserve(matching_->GetMatches().size());

  std::vector<const Branch*> br_vec;
  std::vector<Cuts*> cuts_vec;
  std::vector<int> id_vec;
  br_vec.reserve(branches_.size());
  cuts_vec.reserve(branches_.size());
  id_vec.resize(branches_.size());
  for(const auto& br : branches_) {
    br_vec.emplace_back(br.first);
    cuts_vec.emplace_back(br.second);
  }
  for(auto& ehi : eve_header_indices_) {
    id_vec.at(ehi) = 0;
  }

  for (auto match : matching_->GetMatches(is_inverted_matching_)) {
    id_vec.at(non_eve_header_indices_.at(0)) = match.first;
    id_vec.at(non_eve_header_indices_.at(1)) = match.second;

    if (!ApplyCutOnBranches(br_vec, cuts_vec, id_vec)) continue;
    std::vector<double> temp_vars(vars_.size());
    short i_var{0};
    for (const auto& var : vars_) {
      temp_vars[i_var] = FillVariable(var, br_vec, id_vec);
      i_var++;
    }//variables
    values_.emplace_back(temp_vars);
  }// channels
}

void AnalysisEntry::FillBranchNames() {
  for (auto& var : vars_) {
    const auto& br = var.GetBranches();
    branch_names_.insert(br.begin(), br.end());
  }
  if (cuts_) {
    const auto& br = cuts_->GetBranches();
    branch_names_.insert(br.begin(), br.end());
  }
}

void AnalysisEntry::Init(const Configuration& conf, const std::map<std::string, Matching*>& matches) {
  if (cuts_) {
    cuts_->Init(conf);
  }
  for (auto& var : vars_) {
    var.Init(conf);
  }

  int i{0};
  for(auto& bn : branch_names_) {
    if(conf.GetBranchConfig(bn).GetType() == DetType::kEventHeader) {
      eve_header_indices_.push_back(i);
    } else {
      non_eve_header_indices_.push_back(i);
    }
    i++;
  }
  if(branch_names_.size() == 0) {
    throw std::runtime_error("AnalysisEntry::Init() - at least 1 branch is needed");
  }
  if(non_eve_header_indices_.size() > 2) {
    throw std::runtime_error("AnalysisEntry::Init() - 2 non-EventHeader branches are allowed as maximum");
  }

  if(non_eve_header_indices_.size() == 2) {
    auto match_info = conf.GetMatchInfo(*std::next(branch_names_.begin(), non_eve_header_indices_.at(0)), *std::next(branch_names_.begin(), non_eve_header_indices_.at(1)));
    SetIsInvertedMatching(match_info.second);
    SetMatching((Matching*) matches.find(match_info.first)->second);
  }
}

size_t AnalysisEntry::AddVariable(const Variable& var) {
  auto it = std::find(vars_.begin(), vars_.end(), var);
  if (it == vars_.end()) {// add new variable
    vars_.emplace_back(var);
    return vars_.size() - 1;
  } else {
    return std::distance(vars_.begin(), it);// index of existing
  }
}

}// namespace AnalysisTree
