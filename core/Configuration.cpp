#include <cassert>
#include <iostream>

#include "Configuration.hpp"
#include "Matching.hpp"
#include <TBuffer.h>

namespace AnalysisTree {

void Configuration::Streamer(TBuffer & rb) {
  if (rb.IsReading()) {
    Configuration::Class()->ReadBuffer(rb, this);
    this->matches_index_ = MakeMatchingIndex(matches_);
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

void Configuration::AddMatch(Matching* match) {
  assert(match);

  const std::string br1 = GetBranchConfig(match->GetBranch1Id()).GetName();
  const std::string br2 = GetBranchConfig(match->GetBranch2Id()).GetName();
  const std::string name = br1 + "2" + br2;

  matches_index_.insert(std::make_pair(std::array<std::string, 2>{br1, br2}, name));
}

}// namespace AnalysisTree