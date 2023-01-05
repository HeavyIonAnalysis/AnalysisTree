/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "AnalysisEntry.hpp"

#include "Configuration.hpp"
#include "VariantMagic.hpp"

namespace AnalysisTree {

bool AnalysisEntry::ApplyCutOnBranch(const Branch& br, Cuts* cuts, int i_channel) const {
  if (cuts && !cuts->Apply(br[i_channel])) { return false; }
  return !cuts_ || cuts_->Apply(br[i_channel]);
}

bool AnalysisEntry::ApplyCutOnBranches(const Branch& br1, Cuts* cuts1, int ch1, const Branch& br2, Cuts* cuts2, int ch2) const {
  if (cuts1 && !cuts1->Apply(br1[ch1])) { return false; }
  if (cuts2 && !cuts2->Apply(br2[ch2])) { return false; }
  return !cuts_ || cuts_->Apply(br1[ch1], br1.GetId(), br2[ch2], br2.GetId());
}

bool AnalysisEntry::ApplyCutOnBranches(const Branch& br1, Cuts* cuts1, int ch1, const Branch& br2, Cuts* cuts2, int ch2, const Branch& br3, Cuts* cuts3, int ch3) const {
  if (cuts1 && !cuts1->Apply(br1[ch1])) { return false; }
  if (cuts2 && !cuts2->Apply(br2[ch2])) { return false; }
  if (cuts3 && !cuts3->Apply(br3[ch3])) { return false; }
  return !cuts_ || cuts_->Apply(br1[ch1], br1.GetId(), br2[ch2], br2.GetId(), br3[ch3], br3.GetId());
}

double AnalysisEntry::FillVariable(const Variable& var, const Branch& br1, int ch1, const Branch& br2, int ch2) {
  return var.GetValue(br1[ch1], br1.GetId(), br2[ch2], br2.GetId());
}

double AnalysisEntry::FillVariable(const Variable& var, const Branch& br1, int ch1, const Branch& br2, int ch2, const Branch& br3, int ch3) {
  return var.GetValue(br1[ch1], br1.GetId(), br2[ch2], br2.GetId(), br3[ch3], br3.GetId());
}

/**
 * @brief takes BranchReader and evaluates all Variables associated with branch.
 * If a branch is a Channel or Tracking detector, evaluation is performed channel-by-channel.
 * If channel or track fails to pass cuts it won't be written
 */
void AnalysisEntry::FillFromOneBranch() {
  const auto& br = branches_.at(0).first;

  const auto n_channels = br.size();
  values_.reserve(n_channels);

  for (size_t i_channel = 0; i_channel < n_channels; ++i_channel) {
    if (!ApplyCutOnBranch(br, branches_.at(0).second, i_channel)) continue;
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
  matching_->Clear();
  for (size_t i = 0; i < br1.size(); ++i) {
    matching_->AddMatch(i, 0);
  }
}

/**
 * @brief FillFromTwoBranches populates Variables if matching between two branches is defined
 * It iterates over registered matches and fills variables
 */
void AnalysisEntry::FillFromTwoBranches() {
  auto& br1 = branches_.at(0);
  auto& br2 = branches_.at(1);
  if (br1.first.GetBranchType() == DetType::kEventHeader) {//put EventHeader to second place, to be able to fill matching
    std::swap(br1, br2);
  }
  if (br2.first.GetBranchType() == DetType::kEventHeader) {
    FillMatchingForEventHeader(br1.first, br2.first);
  }
  values_.reserve(matching_->GetMatches().size());

  for (auto match : matching_->GetMatches(is_inverted_matching_)) {
    const int ch1 = match.first;
    const int ch2 = match.second;
    if (!ApplyCutOnBranches(br1.first, br1.second, ch1, br2.first, br2.second, ch2)) continue;
    std::vector<double> temp_vars(vars_.size());
    short i_var{};
    for (const auto& var : vars_) {
      temp_vars[i_var] = FillVariable(var, br1.first, ch1, br2.first, ch2);
      i_var++;
    }//variables
    values_.emplace_back(temp_vars);
  }
}

void AnalysisEntry::FillFromThreeBranches() {
  auto& br1 = branches_.at(0);
  auto& br2 = branches_.at(1);
  auto& br3 = branches_.at(2);
  values_.reserve(matching_->GetMatches().size());
  int event_header_n  = -1;
  if(br1.first.GetBranchType() == DetType::kEventHeader) {
    event_header_n = 0;
  } else if(br2.first.GetBranchType() == DetType::kEventHeader) {
    event_header_n = 1;
  } else {
    event_header_n = 2;
  }
  const int non_event_header_1 = (2-event_header_n)/2;
  const int non_event_header_2 = (5-event_header_n)/2;

  for (auto match : matching_->GetMatches(is_inverted_matching_)) {
    std::array<int, 3> ch;
    ch.at(non_event_header_1) = match.first;
    ch.at(non_event_header_2) = match.second;
    ch.at(event_header_n) = 0;
    if (!ApplyCutOnBranches(br1.first, br1.second, ch.at(0), br2.first, br2.second, ch.at(1), br3.first, br3.second, ch.at(2))) continue;
    std::vector<double> temp_vars(vars_.size());
    short i_var{};
    for (const auto& var : vars_) {
      temp_vars[i_var] = FillVariable(var, br1.first, ch.at(0), br2.first, ch.at(1), br3.first, ch.at(2));
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
  } else if (branches_.size() == 3) {
    FillFromThreeBranches();
  } else {
    throw std::runtime_error("AnalysisEntry::FillValues - branches_.size() is more than 3 or 0");
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

void AnalysisEntry::Init(const Configuration& conf, const std::map<std::string, Matching*>& matches) {
  if (cuts_) {
    cuts_->Init(conf);
  }
  for (auto& var : vars_) {
    var.Init(conf);
  }

  if (branch_names_.size() == 2) {
    auto det1_type = conf.GetBranchConfig(*branch_names_.begin()).GetType();
    auto det2_type = conf.GetBranchConfig(*std::next(branch_names_.begin(), 1)).GetType();

    if (det1_type != DetType::kEventHeader && det2_type != DetType::kEventHeader) {
      auto match_info = conf.GetMatchInfo(*branch_names_.begin(), *std::next(branch_names_.begin(), 1));
      SetIsInvertedMatching(match_info.second);
      SetMatching((Matching*) matches.find(match_info.first)->second);
    } else {
      if (det1_type == DetType::kEventHeader) {
        matching_ = new Matching(branches_.at(1).first.GetId(), branches_.at(0).first.GetId());
      } else {
        matching_ = new Matching(branches_.at(0).first.GetId(), branches_.at(1).first.GetId());
      }
    }
  } else if (branch_names_.size() > 2) {
    std::array<DetType, 3> det_type;
    det_type.at(0) = conf.GetBranchConfig(*branch_names_.begin()).GetType();
    det_type.at(1) = conf.GetBranchConfig(*std::next(branch_names_.begin(), 1)).GetType();
    det_type.at(2) = conf.GetBranchConfig(*std::next(branch_names_.begin(), 2)).GetType();

    int n_event_headers=0;
    int event_header_n=-1;
    for(int i=0; i<3; i++) {
      if(det_type.at(i) == DetType::kEventHeader) {
        n_event_headers++;
        event_header_n = i;
      }
    }
    if(n_event_headers != 1) {
      throw std::runtime_error("AnalysisEntry::Init() - among 3 branches 1 and only 1 must be EventHeader");
    }
    const int non_event_header_1 = (2-event_header_n)/2;
    const int non_event_header_2 = (5-event_header_n)/2;
    auto match_info = conf.GetMatchInfo(*std::next(branch_names_.begin(), non_event_header_1), *std::next(branch_names_.begin(), non_event_header_2));
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
