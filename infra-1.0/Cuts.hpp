#ifndef ANALYSISTREE_CUTS_H
#define ANALYSISTREE_CUTS_H

#include <set>
#include <string>
#include <utility>
#include <vector>

#include <AnalysisTree/Constants.hpp>
#include "SimpleCut.hpp"

// Logical AND is applied for all Cuts in the vector

namespace AnalysisTree{
class Configuration;
}

namespace AnalysisTree::Version1 {
/**
 * @brief Cuts holds list of SimpleCuts and provides Apply function which subsequently applies cuts
 */
class Cuts {

 public:
  Cuts() = default;
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

  Cuts(std::string name, std::initializer_list<SimpleCut> cuts) : name_(std::move(name)),
                                                                  cuts_(cuts.begin(), cuts.end()) {
    for (const auto& v : cuts_) {
      const auto& br = v.GetBranches();
      branch_names_.insert(br.begin(), br.end());
    }
  }

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

  void Init(const AnalysisTree::Configuration& conf);
  void Print() const;

  ANALYSISTREE_ATTR_NODISCARD const std::set<std::string>& GetBranches() const { return branch_names_; }
  ANALYSISTREE_ATTR_DEPRECATED()
  ANALYSISTREE_ATTR_NODISCARD const std::string&
  GetBranchName() const {
    assert(branch_names_.size() == 1);
    return *branch_names_.begin();
  }

  ANALYSISTREE_ATTR_NODISCARD std::set<short> GetBranchIds() const { return branch_ids_; }
  ANALYSISTREE_ATTR_NODISCARD const std::string& GetName() const { return name_; }

  std::vector<SimpleCut>& GetCuts() { return cuts_; }

  friend bool operator==(const Cuts& that, const Cuts& other);
  static bool Equal(const Cuts* that, const Cuts* other);

 protected:
  std::string name_;
  std::set<std::string> branch_names_{};
  std::set<short> branch_ids_{};
  std::vector<SimpleCut> cuts_{};

  bool is_init_{false};

  ClassDef(AnalysisTree::Version1::Cuts, 1)
};

}// namespace AnalysisTree::Version1
#endif//ANALYSISTREE_CUTS_H
