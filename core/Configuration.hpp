
#ifndef ANALYSISTREE_CONFIGURATION_H
#define ANALYSISTREE_CONFIGURATION_H

#include <map>
#include <string>
#include <vector>

#include <RtypesCore.h>
#include <TObject.h>

#include "BranchConfig.hpp"
#include "Constants.hpp"

namespace AnalysisTree {

class Configuration : public TObject {

 public:
  Configuration() = default;
  Configuration(const Configuration &) = default;
  Configuration(Configuration &&) = default;
  Configuration &operator=(Configuration &&) = default;
  Configuration &operator=(const Configuration &) = default;

  void AddBranchConfig(BranchConfig branch) {
    branch.SetId(branches_.size());
    branches_.push_back(branch);
  }

  void AddBranchConfig(const std::string &name, DetType type) {
    BranchConfig branch(name, type);
    branch.SetId(branches_.size());
    branches_.push_back(branch);
  }

  const BranchConfig &GetBranchConfig(Integer_t i) const { return branches_.at(i); }
  const std::vector<BranchConfig> &GetBranchConfigs() const { return branches_; }

  uint GetNumberOfBranches() const { return branches_.size(); }

  BranchConfig &GetBranchConfig(const std::string &name) {
    for (auto &branch : branches_) {
      if (branch.GetName() == name)
        return branch;
    }
    throw std::logic_error("Configuration::GetBranchConfig - no branch " + name);
  }

  const BranchConfig &GetBranchConfig(const std::string &name) const {
    for (const auto &branch : branches_) {
      if (branch.GetName() == name)
        return branch;
    }
    throw std::runtime_error("Configuration::GetBranchConfig - no branch " + name);
  }

  void Print() const {
    std::cout << "This is a " << name_ << std::endl;
    std::cout << "The Tree has the following branches:" << std::endl;
    for (const auto &branch : branches_) {
      std::cout << std::endl;
      branch.Print();
    }
    std::cout << std::endl;
    std::cout << "Matching between branches available" << std::endl;
    for (const auto &match : matches_) {
      std::cout << "  " << match.first[0] << " " << match.first[1]
                << " in branch " << match.second << std::endl;
    }
  }

  const std::string &GetMatchName(const std::string &br1, const std::string &br2) const {
    auto search = matches_.find({br1, br2});
    if (search != matches_.end()) {
      return search->second;
    } else {
      throw std::runtime_error("Configuration::GetMatchName - Not found for branches " + br1 + " and " + br2);
    }
  }

  //TODO fix this logic, looks ugly
  std::pair<std::string, bool> GetMatchInfo(const std::string &br1, const std::string &br2) const {
    auto search = matches_.find({br1, br2});
    if (search != matches_.end()) {
      return std::make_pair(search->second, false);
    } else {
      auto search_1 = matches_.find({br2, br1});
      if (search_1 != matches_.end()) {
        return std::make_pair(search_1->second, true);
      } else {
        throw std::runtime_error("Configuration::GetMatchInfo - Not found for branches " + br1 + " and " + br2);
      }
    }
  }

  void AddMatch(const std::string &br1, const std::string &br2, const std::string &name) {
    matches_.insert(std::make_pair(std::array<std::string, 2>{br1, br2}, name));
  }

  const std::map<std::array<std::string, 2>, std::string> &GetMatches() const { return matches_; }

  void SetName(const std::string &name) { name_ = name; }
  uint GetLastId() const { return branches_.back().GetId(); }

 protected:
  std::string name_{""};
  std::vector<BranchConfig> branches_{};

  std::map<std::array<std::string, 2>, std::string> matches_{};

  ClassDef(AnalysisTree::Configuration, 1)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_CONFIGURATION_H
