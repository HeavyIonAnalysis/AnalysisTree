/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */

#ifndef ANALYSISTREE_CONFIGURATION_H
#define ANALYSISTREE_CONFIGURATION_H

#include <array>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <TObject.h>

#include "BranchConfig.hpp"

namespace AnalysisTree {

class Matching;

/// Information to store about Matching
class MatchingConfig {
 public:
  MatchingConfig() = default;
  MatchingConfig(std::string Branch1,
                 std::string Branch2,
                 std::string DataBranch) : branch1_(std::move(Branch1)), branch2_(std::move(Branch2)), data_branch_(std::move(DataBranch)) {}

  [[nodiscard]] std::string GetFirstBranchName() const { return branch1_; }
  [[nodiscard]] std::string GetSecondBranchName() const { return branch2_; }
  [[nodiscard]] std::string GetDataBranchName() const { return data_branch_; }

 private:
  std::string branch1_;
  std::string branch2_;
  std::string data_branch_;

  ClassDefNV(MatchingConfig, 2);
};

/// Some ROOT magic, ask Eugeny
class Configuration_v3 : public TObject {
 public:
  std::string name_;
  std::map<size_t, BranchConfig> branches_{};
  std::map<std::array<std::string, 2>, std::string> matches_{};

  ClassDef(Configuration_v3, 1);
};

/// A class to store configuration of the whole AnalysisTree object
/**
 * Consists of map with configs for all branches and Matching information.
 * Should be written to ROOT file next to the TTree.
 * Needed to read information correctly and decode meaning of the Container objects
 */
class Configuration : public TObject {

 public:
  using MatchingIndex = std::map<std::array<std::string, 2>, std::string>;

  Configuration() = default;
  explicit Configuration(std::string name) : name_(std::move(name)){};
  Configuration(const Configuration&) = default;
  Configuration(Configuration&&) = default;
  Configuration& operator=(Configuration&&) = default;
  Configuration& operator=(const Configuration&) = default;

  void AddBranchConfig(const BranchConfig& branch) {
    branches_.emplace(branch.GetId(), branch);
  }

  void RemoveBranchConfig(const std::string& branchname);

  [[nodiscard]] std::vector<std::string> GetMatchesOfBranch(const std::string& branchname) const;

  void AddMatch(Matching* match);

  void AddMatch(const MatchingConfig& matching_config);

  ANALYSISTREE_ATTR_NODISCARD BranchConfig& GetBranchConfig(const std::string& name);
  ANALYSISTREE_ATTR_NODISCARD const BranchConfig& GetBranchConfig(const std::string& name) const;
  ANALYSISTREE_ATTR_NODISCARD const BranchConfig& GetBranchConfig(size_t i) const {
    auto it = branches_.find(i);
    if (it == branches_.end()) {
      throw std::runtime_error("Branch with id = " + std::to_string(i) + " not found");
    }
    return it->second;
  }
  ANALYSISTREE_ATTR_NODISCARD const std::map<size_t, BranchConfig>& GetBranchConfigs() const { return branches_; }
  ANALYSISTREE_ATTR_NODISCARD const std::vector<MatchingConfig>& GetMatchingConfigs() const { return matches_; }
  ANALYSISTREE_ATTR_NODISCARD unsigned int GetNumberOfBranches() const { return branches_.size(); }

  ANALYSISTREE_ATTR_NODISCARD const std::string& GetMatchName(const std::string& br1, const std::string& br2) const;
  ANALYSISTREE_ATTR_NODISCARD std::pair<std::string, bool> GetMatchInfo(const std::string& br1, const std::string& br2) const;
  ANALYSISTREE_ATTR_NODISCARD const MatchingIndex& GetMatches() const { return matches_index_; }

  void Print(Option_t* = "") const;

  static MatchingIndex MakeMatchingIndex(const std::vector<MatchingConfig>& matches) {
    MatchingIndex result;
    for (auto& match : matches) {
      std::array<std::string, 2> map_key{match.GetFirstBranchName(), match.GetSecondBranchName()};
      auto emplace_result = result.emplace(map_key, match.GetDataBranchName());
      if (!emplace_result.second) {
        throw std::runtime_error("Two matches with same first and second branch added");
      }
    }
    return result;
  }

  static std::vector<MatchingConfig> MakeMatchConfigsFromIndex(const MatchingIndex& matching_index) {
    std::vector<MatchingConfig> result;
    for (auto& matching_index_element : matching_index) {
      result.emplace_back(matching_index_element.first[0],
                          matching_index_element.first[1],
                          matching_index_element.second);
    }
    return result;
  }

  [[nodiscard]] std::vector<std::string> GetListOfBranches() const;

  /**
    * @brief Merge two configurations without reindexing of the branches
    * THIS FUNCTION IS USED IN THE Infra v1
    * @param other
    */
  void Merge(const Configuration& other) {
    for (auto& other_branch : other.branches_) {
      const auto other_id = other_branch.second.GetId();
      const auto other_name = other_branch.second.GetName();
      for (auto& local_branch : branches_) {
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
  std::vector<MatchingConfig> matches_{};

  MatchingIndex matches_index_{};//! transient field in this version

  void AddMatch(const std::string& br1, const std::string& br2, const std::string& data_branch);

  ClassDef(Configuration, 4)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_CONFIGURATION_H
