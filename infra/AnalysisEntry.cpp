/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "AnalysisEntry.hpp"

#include "Configuration.hpp"
#include "VariantMagic.hpp"

namespace AnalysisTree {

bool AnalysisEntry::ApplyCutOnBranch(const Branch& br, int i_channel) const {
//  if (!br.ApplyCut(i_channel)) return false;
//  if (!cuts_) return true;
//  return ANALYSISTREE_UTILS_VISIT(apply_cut(i_channel, cuts_), br.GetData());
return true;
}

bool AnalysisEntry::ApplyCutOnBranches(const Branch& br1, int ch1, const Branch& br2, int ch2) const {
//  if (!br1.ApplyCut(ch1)) return false;
//  if (!br2.ApplyCut(ch2)) return false;
//
//  if (!cuts_) return true;
//  return ANALYSISTREE_UTILS_VISIT(apply_cut_2_branches(ch1, ch2, cuts_), br1.GetData(), br2.GetData());
return true;
}

double AnalysisEntry::FillVariable(const Variable& var, const Branch& br1, int ch1, const Branch& br2, int ch2) {
  return var.GetValue(br1[ch1], br1.GetId(), br2[ch2], br2.GetId());
//  return ANALYSISTREE_UTILS_VISIT(fill_2_branches(var, ch1, ch2), br1.GetData(), br2.GetData());
}

/**
 * @brief takes BranchReader and evaluates all Variables associated with branch.
 * If a branch is a Channel or Tracking detector, evaluation is performed channel-by-channel.
 * If channel or track fails to pass cuts it won't be written
 */
void AnalysisEntry::FillFromOneBranch() {
  const Branch& br = branches_.at(0);
  const auto n_channels = br.size();
  values_.reserve(n_channels);

  for (size_t i_channel = 0; i_channel < n_channels; ++i_channel) {
    if (!ApplyCutOnBranch(br, i_channel)) continue;
    std::vector<double> temp_vars(vars_.size());
    short i_var{0};
    for (const auto& var : vars_) {
      temp_vars[i_var] = var.GetValue(br[i_channel]);
      i_var++;
    }//variables
    values_.emplace_back(temp_vars);
  }//channels
}

void AnalysisEntry::FillMatchingForEventHeader(const Branch& br1, const Branch& br2) {
  matching_ = new Matching(br1.GetId(), br2.GetId());
  for (size_t i = 0; i < br1.size(); ++i) {
    matching_->AddMatch(i, 0);
  }
}

/**
 * @brief FillFromTwoBranches populates Variables if matching between two branches is defined
 * It iterates over registered matches and fills variables
 */
void AnalysisEntry::FillFromTwoBranches() {
  Branch& br1 = branches_.at(0);
  Branch& br2 = branches_.at(1);
  if (br1.GetBranchType() == DetType::kEventHeader) {//put EventHeader to second place, to be able to fill matching
    std::swap(br1, br2);
  }
  if (br2.GetBranchType() == DetType::kEventHeader) {
    FillMatchingForEventHeader(br1, br2);
  }
  values_.reserve(matching_->GetMatches().size());

  for (auto match : matching_->GetMatches(is_inverted_matching_)) {
    const int ch1 = match.first;
    const int ch2 = match.second;
    if (!ApplyCutOnBranches(br1, ch1, br2, ch2)) continue;
    std::vector<double> temp_vars(vars_.size());
    short i_var{};
    for (const auto& var : vars_) {
      temp_vars[i_var] = FillVariable(var, br1, ch1, br2, ch2);
      i_var++;
    }//variables
    values_.emplace_back(temp_vars);
  }
}

void AnalysisEntry::FillValues() {
  values_.clear();
  if (branches_.size() == 1) {
    FillFromOneBranch();
  } else if (branches_.size() == 2) {
    FillFromTwoBranches();
  } else {
    throw std::runtime_error("AnalysisEntry::FillValues - branches_.size() is more than 2 or 0");
  }
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

void AnalysisEntry::Init(const AnalysisTree::Configuration& conf, const std::map<std::string, Matching*>& matches) {
  if (cuts_) {
    cuts_->Init(conf);
  }
  for (auto& var : vars_) {
    var.Init(conf);
  }

  if (branch_names_.size() > 1) {
    auto det1_type = conf.GetBranchConfig(*branch_names_.begin()).GetType();
    auto det2_type = conf.GetBranchConfig(*std::next(branch_names_.begin(), 1)).GetType();

    if (det1_type != DetType::kEventHeader && det2_type != DetType::kEventHeader) {
      auto match_info = conf.GetMatchInfo(*branch_names_.begin(), *std::next(branch_names_.begin(), 1));
      SetIsInvertedMatching(match_info.second);
      SetMatching((Matching*) matches.find(match_info.first)->second);
    }
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