
#ifndef ANALYSISTREE_CONFIGURATION_H
#define ANALYSISTREE_CONFIGURATION_H

#include <array>
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
  explicit Configuration(std::string name) : name_(std::move(name)){};
  Configuration(const Configuration&) = default;
  Configuration(Configuration&&) = default;
  Configuration& operator=(Configuration&&) = default;
  Configuration& operator=(const Configuration&) = default;

  void AddBranchConfig(const BranchConfig& branch) {
    branches_.emplace(branch.GetId(), branch);
  }

  void AddMatch(Matching* match);

  ANALYSISTREE_ATTR_NODISCARD BranchConfig& GetBranchConfig(const std::string& name);
  ANALYSISTREE_ATTR_NODISCARD const BranchConfig& GetBranchConfig(const std::string& name) const;
  ANALYSISTREE_ATTR_NODISCARD const BranchConfig& GetBranchConfig(size_t i) const {
    auto it = branches_.find(i);
    if(it == branches_.end()){
      throw std::runtime_error("Branch with id = " + std::to_string(i) + " not found");
    }
    return it->second;
  }
  ANALYSISTREE_ATTR_NODISCARD const std::map<size_t, BranchConfig>& GetBranchConfigs() const { return branches_; }
  ANALYSISTREE_ATTR_NODISCARD unsigned int GetNumberOfBranches() const { return branches_.size(); }

  ANALYSISTREE_ATTR_NODISCARD const std::string& GetMatchName(const std::string& br1, const std::string& br2) const;
  ANALYSISTREE_ATTR_NODISCARD std::pair<std::string, bool> GetMatchInfo(const std::string& br1, const std::string& br2) const;
  ANALYSISTREE_ATTR_NODISCARD const std::map<std::array<std::string, 2>, std::string>& GetMatches() const { return matches_; }

  void Print(Option_t* = "") const;

  /**
 * @brief Merge two configurations without reindexing of the branches
 * @param other
 */
  void Merge(const Configuration& other) {
    for (auto & other_branch : other.branches_) {
      const auto other_id = other_branch.second.GetId();
      const auto other_name = other_branch.second.GetName();
      for (auto &local_branch : branches_) {
        if (other_id == local_branch.second.GetId()) {
          throw std::runtime_error("Configurations contain branches with the same id-s");
        }
        if (other_name == local_branch.second.GetName()) {
          throw std::runtime_error("Configurations contain branches with the same names");
        }
      }
      /// DO NOT REINDEX
      branches_.emplace(other_branch);
    }
  }


 protected:
  std::string name_;
  std::map<size_t, BranchConfig> branches_{};
  std::map<std::array<std::string, 2>, std::string> matches_{};

  ClassDef(Configuration, 3)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_CONFIGURATION_H
