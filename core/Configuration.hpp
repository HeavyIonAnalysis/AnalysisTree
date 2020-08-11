
#ifndef ANALYSISTREE_CONFIGURATION_H
#define ANALYSISTREE_CONFIGURATION_H

#include <map>
#include <string>
#include <vector>

#include <TObject.h>

#include "BranchConfig.hpp"

namespace AnalysisTree {

class Matching;

class Configuration : public TObject {

 public:
  Configuration() = default;
  explicit Configuration(std::string name) : name_(std::move(name)) {};
  Configuration(const Configuration &) = default;
  Configuration(Configuration &&) = default;
  Configuration &operator=(Configuration &&) = default;
  Configuration &operator=(const Configuration &) = default;

  void AddBranchConfig(BranchConfig branch) {
    branch.SetId(branches_.size());
    branches_.emplace_back(branch);
  }

  void AddMatch(Matching* match);

  [[nodiscard]] BranchConfig &GetBranchConfig(const std::string &name);
  [[nodiscard]] const BranchConfig &GetBranchConfig(const std::string &name) const;
  [[nodiscard]] const BranchConfig &GetBranchConfig(Integer_t i) const { return branches_.at(i); }
  [[nodiscard]] const std::vector<BranchConfig> &GetBranchConfigs() const { return branches_; }
  [[nodiscard]] uint GetNumberOfBranches() const { return branches_.size(); }

  [[nodiscard]] uint GetLastId() const { return branches_.back().GetId(); }

  [[nodiscard]] const std::string &GetMatchName(const std::string &br1, const std::string &br2) const;
  [[nodiscard]] std::pair<std::string, bool> GetMatchInfo(const std::string &br1, const std::string &br2) const;
  [[nodiscard]] const std::map<std::array<std::string, 2>, std::string> &GetMatches() const { return matches_; }

  void Print() const;

 protected:
  std::string name_;
  std::vector<BranchConfig> branches_{};
  std::map<std::array<std::string, 2>, std::string> matches_{};

  ClassDef(AnalysisTree::Configuration, 1)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_CONFIGURATION_H
