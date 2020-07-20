
#ifndef ANALYSISTREE_CONFIGURATION_H
#define ANALYSISTREE_CONFIGURATION_H

#include <map>
#include <string>
#include <vector>

#include <TObject.h>

#include "BranchConfig.hpp"
#include "Constants.hpp"
#include "Matching.hpp"

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

  [[deprecated("Please use AddBranchConfig({const std::string &name, DetType type}) for implit convertion to BranchConfig)")]]
  void AddBranchConfig(const std::string &name, DetType type) {
    BranchConfig branch(name, type);
    branch.SetId(branches_.size());
    branches_.push_back(branch);
  }

  void SetName(const std::string &name) { name_ = name; }
  uint GetLastId() const { return branches_.back().GetId(); }

  const BranchConfig &GetBranchConfig(Integer_t i) const { return branches_.at(i); }
  const std::vector<BranchConfig> &GetBranchConfigs() const { return branches_; }

  BranchConfig &GetBranchConfig(const std::string &name);
  const BranchConfig &GetBranchConfig(const std::string &name) const;

  uint GetNumberOfBranches() const { return branches_.size(); }


  const std::string &GetMatchName(const std::string &br1, const std::string &br2) const;
  std::pair<std::string, bool> GetMatchInfo(const std::string &br1, const std::string &br2) const;

  [[deprecated("Please use AddMatch(Matching* m)")]]
  void AddMatch(const std::string &br1, const std::string &br2, const std::string &name) {
    matches_.insert(std::make_pair(std::array<std::string, 2>{br1, br2}, name));
  }

  void AddMatch(Matching* match);

  const std::map<std::array<std::string, 2>, std::string> &GetMatches() const { return matches_; }

  void Print() const;

 protected:
  std::string name_{""};
  std::vector<BranchConfig> branches_{};

  std::map<std::array<std::string, 2>, std::string> matches_{};

  ClassDef(AnalysisTree::Configuration, 1)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_CONFIGURATION_H
