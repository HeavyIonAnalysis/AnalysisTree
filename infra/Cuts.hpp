#ifndef ANALYSISTREE_CUTS_H
#define ANALYSISTREE_CUTS_H

#include <set>
#include <string>
#include <vector>

#include "Constants.hpp"
#include "SimpleCut.hpp"

// Logical AND is applied for all Cuts in the vector

namespace AnalysisTree {

class Configuration;

class Cuts {

 public:
  Cuts() = delete;
  Cuts(const Cuts& cut) = default;
  Cuts(Cuts&& cut) = default;
  Cuts& operator=(Cuts&&) = default;
  Cuts& operator=(const Cuts& cut) = default;
  virtual ~Cuts() = default;

  Cuts(std::string name, std::vector<SimpleCut> cuts) : name_(std::move(name)),
                                                        cuts_(std::move(cuts)) {
    for (const auto& v : cuts_) {
      const auto& br = v.GetBranches();
      branch_names_.insert(br.begin(), br.end());
    }
  };

  template<class T>
  bool Apply(const T& ob) const {
    if (!is_init_) {
      throw std::runtime_error("Cuts::Apply - cut is not initialized!!");
    }
    for (const auto& cut : cuts_) {
      if (!cut.Apply(ob))
        return false;
    }
    return true;
  }

  template<class A, class B>
  bool Apply(const A& a, int a_id, const B& b, int b_id) const {
    if (!is_init_) {
      throw std::runtime_error("Cuts::Apply - cut is not initialized!!");
    }
    for (const auto& cut : cuts_) {
      if (!cut.Apply(a, a_id, b, b_id))
        return false;
    }
    return true;
  }

  void Init(const Configuration& conf);
  void Print() const;

  [[nodiscard]] const std::set<std::string>& GetBranches() const { return branch_names_; }
  [[deprecated]]
  [[nodiscard]] const std::string&
  GetBranchName() const {
    assert(branch_names_.size() == 1);
    return *branch_names_.begin();
  }

  [[nodiscard]] std::set<short> GetBranchIds() const { return branch_ids_; }
  [[nodiscard]] const std::string& GetName() const { return name_; }

  std::vector<SimpleCut>& GetCuts() { return cuts_; }

  friend bool operator==(const Cuts& that, const Cuts& other);
  static bool Equal(const Cuts* that, const Cuts* other);

 protected:
  std::string name_;
  std::set<std::string> branch_names_{};
  std::set<short> branch_ids_{};
  std::vector<SimpleCut> cuts_{};

  bool is_init_{false};

  ClassDef(AnalysisTree::Cuts, 1)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_CUTS_H
