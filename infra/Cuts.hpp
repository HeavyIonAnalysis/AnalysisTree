#ifndef ANALYSISTREE_CUTS_H
#define ANALYSISTREE_CUTS_H

#include <string>
#include <vector>
#include <set>

#include "Constants.hpp"
#include "SimpleCut.hpp"

// Logical AND is applied for all Cuts in the vector

namespace AnalysisTree {

class Configuration;

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

  template<class T>
  bool Apply(const T &ob) const {
    if (!is_init_) {
      throw std::runtime_error("Cuts::Apply - cut is not initialized!!");
    }
    for (const auto &cut : cuts_) {
      if (!cut.Apply(ob))
        return false;
    }
    return true;
  }

  template<class A, class B>
  bool Apply(const A &a, int a_id, const B &b, int b_id) const {
    if (!is_init_) {
      throw std::runtime_error("Cuts::Apply - cut is not initialized!!");
    }
    for (const auto &cut : cuts_) {
      if (!cut.Apply(a, a_id, b, b_id))
        return false;
    }
    return true;
  }

  void AddCut(const SimpleCut &cut) { cuts_.push_back(cut); }
  void AddCuts(const std::vector<SimpleCut> &cuts) { cuts_ = cuts; }

  void Init(const Configuration &conf);

//  void SetName(const std::string &name) { name_ = name; }
  const std::set<std::string> &GetBranches() const { return branch_names_; }
  [[deprecated]]
  const std::string &GetBranchName() const { assert(branch_names_.size()==1); return *branch_names_.begin(); }

  void Print() const;
  [[deprecated]]
  int GetBranchId() const { return branch_id_; }
  const std::string &GetName() const { return name_; }

  std::vector<SimpleCut> &GetCuts() { return cuts_; }

 protected:
  std::string name_{""};
//  std::vector<std::string> branch_names_{};
  std::set<std::string> branch_names_{};
  std::vector<SimpleCut> cuts_{};

  int branch_id_{-1};
  bool is_init_{false};

  ClassDef(AnalysisTree::Cuts, 1)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_CUTS_H
