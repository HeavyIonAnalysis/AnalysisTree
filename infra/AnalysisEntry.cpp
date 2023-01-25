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

bool AnalysisEntry::ApplyCutOnBranches(std::vector<const Branch*>& br, std::vector<Cuts*>& cuts, std::vector<int>& ch) const {
  if(br.size() != cuts.size() || cuts.size() != ch.size()) {
    throw std::runtime_error("AnalysisTree::AnalysisEntry::ApplyCutOnBranches() - Branch, Cuts and Ch vectors must have the same size");
  }
  std::vector<const BranchChannel*> bch_vec;
  std::vector<size_t> id_vec;
  bch_vec.reserve(br.size());
  id_vec.reserve(br.size());
  for(int i=0; i<br.size(); i++) {
    BranchChannel bch = (*br.at(i))[ch.at(i)];
    if(cuts.at(i) && !cuts.at(i)->Apply(bch)) { return false; }
    BranchChannel* bchptr = new BranchChannel(std::move(bch));
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

// bool AnalysisEntry::ApplyCutOnBranches(const Branch& br1, Cuts* cuts1, int ch1, const Branch& br2, Cuts* cuts2, int ch2, const Branch& br3, Cuts* cuts3, int ch3) const {
//   std::vector<std::tuple<const Branch*, Cuts*, int>> vec = {{&br1, cuts1, ch1}, {&br2, cuts2, ch2}, {&br3, cuts3, ch3}};
//   return ApplyCutOnBranches(vec);
// }

double AnalysisEntry::FillVariable(const Variable& var, std::vector<const Branch*>& br, std::vector<int>& id) {
  if(br.size() != id.size()) {
    throw std::runtime_error("AnalysisTree::AnalysisEntry::FillVariable() - Branch and Id vectors must have the same size");
  }
  std::vector<const BranchChannel*> bch_vec;
  std::vector<size_t> id_vec;
  bch_vec.reserve(br.size());
  id_vec.reserve(br.size());
  for(int i=0; i<br.size(); i++) {
    BranchChannel bch = (*br.at(i))[id.at(i)];
    BranchChannel* bchptr = new BranchChannel(std::move(bch));
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

// double AnalysisEntry::FillVariable(const Variable& var, const Branch& br1, int ch1, const Branch& br2, int ch2, const Branch& br3, int ch3) {
//   std::vector<std::pair<const Branch*, int>> vec = {{&br1, ch1}, {&br2, ch2}, {&br3, ch3}};
//   return FillVariable(var, vec);
// }

///**
// * @brief takes BranchReader and evaluates all Variables associated with branch.
// * If a branch is a Channel or Tracking detector, evaluation is performed channel-by-channel.
// * If channel or track fails to pass cuts it won't be written
// */
// void AnalysisEntry::FillFromOneBranch() {
//   const auto& br = branches_.at(0).first;
//
//   const auto n_channels = br.size();
//   values_.reserve(n_channels);
//
//   for (size_t i_channel = 0; i_channel < n_channels; ++i_channel) {
//     if (!ApplyCutOnBranch(br, branches_.at(0).second, i_channel)) continue;
//     std::vector<double> temp_vars(vars_.size());
//     short i_var{0};
//     for (const auto& var : vars_) {
//       temp_vars[i_var] = var.GetValue(br[i_channel]);
//       i_var++;
//     }//variables
//     values_.emplace_back(temp_vars);
//   }//channels
// }

// void AnalysisEntry::FillMatchingForEventHeader(const Branch& br1, const Branch& br2) {
//   matching_->Clear();
//   for (size_t i = 0; i < br1.size(); ++i) {
//     matching_->AddMatch(i, 0);
//   }
// }

///**
//* @brief FillFromTwoBranches populates Variables if matching between two branches is defined
//* It iterates over registered matches and fills variables
//*/
// void AnalysisEntry::FillFromTwoBranches() {
//   auto& br1 = branches_.at(0);
//   auto& br2 = branches_.at(1);
//   if (br1.first.GetBranchType() == DetType::kEventHeader) {//put EventHeader to second place, to be able to fill matching
//     std::swap(br1, br2);
//   }
//   if (br2.first.GetBranchType() == DetType::kEventHeader) {
//     FillMatchingForEventHeader(br1.first, br2.first);
//   }
//   values_.reserve(matching_->GetMatches().size());
//
//   for (auto match : matching_->GetMatches(is_inverted_matching_)) {
//     const int ch1 = match.first;
//     const int ch2 = match.second;
//     if (!ApplyCutOnBranches(br1.first, br1.second, ch1, br2.first, br2.second, ch2)) continue;
//     std::vector<double> temp_vars(vars_.size());
//     short i_var{};
//     for (const auto& var : vars_) {
//       temp_vars[i_var] = FillVariable(var, br1.first, ch1, br2.first, ch2);
//       i_var++;
//     }//variables
//     values_.emplace_back(temp_vars);
//   }
// }

// void AnalysisEntry::FillFromThreeBranches() {
//   auto& br1 = branches_.at(0);
//   auto& br2 = branches_.at(1);
//   auto& br3 = branches_.at(2);
//   values_.reserve(matching_->GetMatches().size());
//   int event_header_n  = -1;
//   if(br1.first.GetBranchType() == DetType::kEventHeader) {
//     event_header_n = 0;
//   } else if(br2.first.GetBranchType() == DetType::kEventHeader) {
//     event_header_n = 1;
//   } else {
//     event_header_n = 2;
//   }
//   const int non_event_header_1 = (2-event_header_n)/2;
//   const int non_event_header_2 = (5-event_header_n)/2;
//
//   for (auto match : matching_->GetMatches(is_inverted_matching_)) {
//     std::array<int, 3> ch;
//     ch.at(non_event_header_1) = match.first;
//     ch.at(non_event_header_2) = match.second;
//     ch.at(event_header_n) = 0;
//     if (!ApplyCutOnBranches(br1.first, br1.second, ch.at(0), br2.first, br2.second, ch.at(1), br3.first, br3.second, ch.at(2))) continue;
//     std::vector<double> temp_vars(vars_.size());
//     short i_var{};
//     for (const auto& var : vars_) {
//       temp_vars[i_var] = FillVariable(var, br1.first, ch.at(0), br2.first, ch.at(1), br3.first, ch.at(2));
//       i_var++;
//     }//variables
//     values_.emplace_back(temp_vars);
//   }
// }

void AnalysisEntry::FillValues() {
  values_.clear();
  if (non_eve_header_indices_.size() == 0) {
    FillFromEveHeaders();
  } else if (non_eve_header_indices_.size() == 1) {
    FillFromOneChannalizedBranch();
  } else if (non_eve_header_indices_.size() == 2) {
    FillFromTwoChannalizedBranches();
  }
//   values_.clear();
//   if (branches_.size() == 1) {
//     FillFromOneBranch();
//   } else if (branches_.size() == 2) {
//     FillFromTwoBranches();
//   } else if (branches_.size() == 3) {
//     FillFromThreeBranches();
//   } else {
//     throw std::runtime_error("AnalysisEntry::FillValues - branches_.size() is more than 3 or 0");
//   }
}

void AnalysisEntry::FillFromEveHeaders() {
  std::vector<const Branch*> br_vec;
  std::vector<Cuts*> cuts_vec;
  std::vector<int> id_vec;
  br_vec.reserve(branches_.size());
  cuts_vec.reserve(branches_.size());
  id_vec.reserve(branches_.size());
  for(const auto& br : branches_) {
    Branch* br_ptr = new Branch(std::move(br.first));
    br_vec.emplace_back(br_ptr);
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
  for(auto& bv : br_vec) {
    delete bv;
  }
}

void AnalysisEntry::FillFromOneChannalizedBranch() {
  const auto n_channels = branches_.at(non_eve_header_indices_.at(0)).first.size();
  values_.reserve(n_channels);

  std::vector<const Branch*> br_vec;
  std::vector<Cuts*> cuts_vec;
  std::vector<int> id_vec;
  br_vec.reserve(branches_.size());
  cuts_vec.reserve(branches_.size());
  id_vec.resize(branches_.size());
  for(const auto& br : branches_) {
    Branch* br_ptr = new Branch(std::move(br.first));
    br_vec.emplace_back(br_ptr);
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
  for(auto& bv : br_vec) {
    delete bv;
  }
}

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
    Branch* br_ptr = new Branch(std::move(br.first));
    br_vec.emplace_back(br_ptr);
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
  for(auto& bv : br_vec) {
    delete bv;
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
