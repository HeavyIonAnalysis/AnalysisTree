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

class MatchingConfig {
 public:
  MatchingConfig() = default;
  MatchingConfig(std::string Branch1,
                 std::string Branch2,
                 std::string DataBranch) : branch1_(std::move(Branch1)), branch2_(std::move(Branch2)), data_branch_(std::move(DataBranch)) {}

  std::string GetFirstBranchName() const { return branch1_; }
  std::string GetSecondBranchName() const { return branch2_; }
  std::string GetDataBranchName() const { return data_branch_; }

 private:
  std::string branch1_;
  std::string branch2_;
  std::string data_branch_;

  ClassDefNV(MatchingConfig, 2);
};

class Configuration_v3 : public TObject {
 public:
  std::string name_;
  std::map<size_t, BranchConfig> branches_{};
  std::map<std::array<std::string, 2>, std::string> matches_{};

  ClassDef(Configuration_v3, 1);
};

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

  void AddMatch(Matching* match);

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
  ANALYSISTREE_ATTR_NODISCARD unsigned int GetNumberOfBranches() const { return branches_.size(); }

  ANALYSISTREE_ATTR_NODISCARD const std::string& GetMatchName(const std::string& br1, const std::string& br2) const;
  ANALYSISTREE_ATTR_NODISCARD std::pair<std::string, bool> GetMatchInfo(const std::string& br1, const std::string& br2) const;
  ANALYSISTREE_ATTR_NODISCARD const std::map<std::array<std::string, 2>, std::string>& GetMatches() const { return matches_index_; }

  void Print(Option_t* = "") const;

  static std::map<std::array<std::string, 2>, std::string> MakeMatchingIndex(const std::vector<MatchingConfig>& matches) {
    std::map<std::array<std::string, 2>, std::string> result;
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

  std::vector<std::string> GetListOfBranches() const;

  std::vector<std::string> GetListOfBranchesExcluding(const std::vector<std::string>& exclude) const;

  /**
    * @brief Merge two configurations without reindexing of the branches
    * THIS FUNCITON IS USED IN THE Infra v1
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

  ClassDef(Configuration, 4)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_CONFIGURATION_H
