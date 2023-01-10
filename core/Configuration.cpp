/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <string>

#include "Configuration.hpp"
#include "Matching.hpp"

#include <TBuffer.h>
#include <TClass.h>
#include <TROOT.h>
#include <TVirtualStreamerInfo.h>

namespace AnalysisTree {

void Configuration::Streamer(TBuffer& rb) {
  if (rb.IsReading()) {
    bool read_ok = false;
    UInt_t rs = 0, rc = 0;
    auto version_from_file = rb.ReadVersion(&rs, &rc, Configuration::Class());
    if (version_from_file == Class()->GetClassVersion()) {
      Configuration::Class()->ReadBuffer(rb, this, version_from_file, rs, rc);
      // populate the transient field
      this->matches_index_ = MakeMatchingIndex(matches_);
      read_ok = true;
    } else if (version_from_file == 3) {
      //      below structure description for version 3 of this class
      Warning(__func__, "Reading AnalysisTree::Configuration of version %d in compatibility mode. "
                        "Current version of implementation is %d.",
              version_from_file,
              Configuration::Class()->GetClassVersion());
      Configuration_v3 conf_v3;
      Configuration_v3::Class()->ReadBuffer(rb, &conf_v3, 1, rs, rc);
      this->name_ = conf_v3.name_;
      this->branches_ = conf_v3.branches_;
      this->matches_ = MakeMatchConfigsFromIndex(conf_v3.matches_);
      // populate the transient field
      this->matches_index_ = conf_v3.matches_;
      read_ok = true;
    } else {
      Warning(__func__, "Current version of AnalysisTree::Configuration (%d) "
                        "is different from the version from file (%d) and no rule to read this version was implemented. "
                        "Contact developers if dedicated Streamer implementation is needed for this version.",
              Configuration::Class()->GetClassVersion(),
              version_from_file);
      read_ok = false;
    }

  } else {
    this->matches_ = MakeMatchConfigsFromIndex(matches_index_);
    Configuration::Class()->WriteBuffer(rb, this);
  }
}

BranchConfig& Configuration::GetBranchConfig(const std::string& name) {
  for (auto& branch : branches_) {
    if (branch.second.GetName() == name)
      return branch.second;
  }
  throw std::runtime_error("Configuration::GetBranchConfig - no branch " + name);
}

const BranchConfig& Configuration::GetBranchConfig(const std::string& name) const {
  for (auto& branch : branches_) {
    if (branch.second.GetName() == name)
      return branch.second;
  }
  throw std::runtime_error("Configuration::GetBranchConfig - no branch " + name);
}

void Configuration::Print(Option_t*) const {
  std::cout << "This is a " << name_ << std::endl;
  std::cout << "The Tree has the following branches:" << std::endl;
  for (const auto& branch : branches_) {
    std::cout << std::endl;
    branch.second.Print();
  }
  std::cout << std::endl;
  std::cout << "Matching between branches available" << std::endl;
  for (const auto& match : matches_index_) {
    std::cout << "  " << match.first[0] << " " << match.first[1]
              << " in branch " << match.second << std::endl;
  }
}

const std::string& Configuration::GetMatchName(const std::string& br1, const std::string& br2) const {
  auto search = matches_index_.find({br1, br2});
  if (search != matches_index_.end()) {
    return search->second;
  } else {
    throw std::runtime_error("Configuration::GetMatchName - Not found for branches " + br1 + " and " + br2);
  }
}

//TODO fix this logic, looks ugly
std::pair<std::string, bool> Configuration::GetMatchInfo(const std::string& br1, const std::string& br2) const {
  auto search = matches_index_.find({br1, br2});
  if (search != matches_index_.end()) {
    return std::make_pair(search->second, false);
  } else {
    auto search_1 = matches_index_.find({br2, br1});
    if (search_1 != matches_index_.end()) {
      return std::make_pair(search_1->second, true);
    } else {
      throw std::runtime_error("Configuration::GetMatchInfo - Not found for branches " + br1 + " and " + br2);
    }
  }
}

void Configuration::AddMatch(const std::string& br1, const std::string& br2, const std::string& data_branch) {
  /* looking up for match with same branches */
  auto is_matching_exists = std::find_if(begin(matches_), end(matches_), [&br1, &br2](const MatchingConfig& config) -> bool {
                              return (br1 == config.GetFirstBranchName() && br2 == config.GetSecondBranchName()) || (br2 == config.GetFirstBranchName() && br1 == config.GetSecondBranchName());
                            })
      != end(matches_);

  if (is_matching_exists) {
    Warning(__func__, "Matching between branches %s and %s already exists", br1.c_str(), br2.c_str());
    return;
  }

  matches_.emplace_back(br1, br2, data_branch);
  /* refresh index */
  matches_index_ = MakeMatchingIndex(matches_);
}

void Configuration::AddMatch(Matching* match) {
  assert(match);
  const std::string br1 = GetBranchConfig(match->GetBranch1Id()).GetName();
  const std::string br2 = GetBranchConfig(match->GetBranch2Id()).GetName();
  const std::string data_branch = br1 + "2" + br2;

  AddMatch(br1, br2, data_branch);
}

void Configuration::AddMatch(const MatchingConfig& matching_config) {
  const std::string br1 = matching_config.GetFirstBranchName();
  const std::string br2 = matching_config.GetSecondBranchName();
  const std::string data_branch = matching_config.GetDataBranchName();

  AddMatch(br1, br2, data_branch);
}

std::vector<std::string> Configuration::GetListOfBranches() const {
  std::vector<std::string> branches{};
  for (const auto& br : branches_) {
    branches.emplace_back(br.second.GetName());
  }
  for (const auto& match : matches_index_) {
    branches.emplace_back(match.second);
  }
  return branches;
}

void Configuration::RemoveBranchConfig(const std::string& branchname) {
  // Remove branch itself
  for (auto br = branches_.begin(); br != branches_.end();) {
    if (br->second.GetName() == branchname) {
      std::cout << "AnalysisTree::Configuration Removing branch: " << branchname << std::endl;
      br = branches_.erase(br);// reseat iterator to a valid value post-erase
    } else {
      ++br;
    }
  }
  // Remove matchings with this branch
  for (auto ma = matches_index_.begin(); ma != matches_index_.end();) {
    if (ma->first[0] == branchname || ma->first[1] == branchname) {
      std::cout << "AnalysisTree::Configuration Removing matching branch: " << ma->second << std::endl;
      ma = matches_index_.erase(ma);// reseat iterator to a valid value post-erase
    } else {
      ++ma;
    }
  }
  this->matches_ = MakeMatchConfigsFromIndex(matches_index_);
}

std::vector<std::string> Configuration::GetMatchesOfBranch(const std::string& branchname) const {
  std::vector<std::string> matches{};
  for (auto& ma : matches_) {
    if (ma.GetFirstBranchName() == branchname || ma.GetSecondBranchName() == branchname)
      matches.emplace_back(ma.GetDataBranchName());
  }
  return matches;
}

}// namespace AnalysisTree
