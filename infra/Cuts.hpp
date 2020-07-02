#ifndef ANALYSISTREE_CUTS_H
#define ANALYSISTREE_CUTS_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "BranchConfig.hpp"
#include "Configuration.hpp"
#include "Constants.hpp"
#include "SimpleCut.hpp"

// Logical AND is applied for all Cuts in the vector

namespace AnalysisTree {

class Cuts {

 public:
  Cuts() = default;
  Cuts(const Cuts &cut) = default;
  Cuts(Cuts &&cut) = default;
  Cuts &operator=(Cuts &&) = default;
  Cuts &operator=(const Cuts &cut) = default;
  virtual ~Cuts() = default;

  explicit Cuts(std::string name) : name_(std::move(name)){};
  Cuts(std::string name, std::vector<SimpleCut> cuts) : name_(std::move(name)),
                                                        cuts_(std::move(cuts)){};

  template<typename T>
  bool Apply(const T &ob) const {
    if (!is_init_) {
      std::cout << "Cuts::Apply - cut is not initialized!!" << std::endl;
      exit(EXIT_FAILURE);
    }
    for (const auto &cut : cuts_) {
      if (!cut.Apply(ob))
        return false;
    }
    return true;
  }

  void AddCut(const SimpleCut &cut) { cuts_.push_back(cut); }
  void AddCuts(const std::vector<SimpleCut> &cuts) { cuts_ = cuts; }

  void Init(const Configuration &conf) {
    for (auto &cut : cuts_) {
      for (auto &var : cut.Variables()) {
        var.Init(conf);
      }
    }
    is_init_ = true;
  }

  void SetName(const std::string &name) { name_ = name; }
  const std::string &GetBranchName() const { return cuts_[0].GetVariables()[0].GetBranchName(); }//TODO fix this

  void Print() const {
    std::cout << "Cut " << name_ << " defined as:" << std::endl;
    for (const auto &cut : cuts_) {
      cut.Print();
    }
  }
  int GetBranchId() const { return branch_id_; }
  const std::string &GetName() const { return name_; }

  std::vector<SimpleCut> &GetCuts() { return cuts_; }

 protected:
  std::string name_{""};
  std::string branch_name_{""};
  std::vector<SimpleCut> cuts_{};

  int branch_id_{-1};
  bool is_init_{false};

  ClassDef(AnalysisTree::Cuts, 1)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_CUTS_H
