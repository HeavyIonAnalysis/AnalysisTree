
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

struct StringArray : public std::array<std::string, 2>
{
  StringArray() = default;
  StringArray(std::string f, std::string s) {
    this->at(0) = std::move(f);
    this->at(0) = std::move(s);
  }
  ~StringArray() = default;
 private:
 ClassDef(StringArray, 1);
};

//struct StringArray
//{
//  StringArray() = default;
//  StringArray(std::string f, std::string s) {
//    array_[0] = std::move(f);
//    array_[1] = std::move(s);
//  }
//
//  bool operator<(const StringArray &c2) const {
//    return array_ < c2.array_;
//  }
//  const std::string& operator[](std::size_t idx) const {
//    return array_[idx];
//  }
//  std::array<std::string, 2> array_{};
//};

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
  ANALYSISTREE_ATTR_NODISCARD const std::map<StringArray, std::string>& GetMatches() const { return matches_; }

  void Print(Option_t* = "") const;

 protected:
  std::string name_;
  std::map<size_t, BranchConfig> branches_{};
  std::map<StringArray, std::string> matches_{};

  ClassDef(Configuration, 3)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_CONFIGURATION_H
