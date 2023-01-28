/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_CUTS_H
#define ANALYSISTREE_CUTS_H

#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "Constants.hpp"
#include "SimpleCut.hpp"
/**
 * @brief Cuts keep list of SimpleCuts. Logical AND is applied for all SimpleCut in the Cuts object
 */
namespace AnalysisTree {

class Configuration;

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

  /**
   * @brief Evaluates all SimpleCuts
   * @tparam T type of data-object associated with TTree
   * @param object
   * @return result of cut
   */
  template<class T>
  bool Apply(const T& object) const {
    if (!is_init_) {
      throw std::runtime_error("Cuts::Apply - cut is not initialized!!");
    }
    for (const auto& cut : cuts_) {
      if (!cut.Apply(object))
        return false;
    }
    return true;
  }

  bool Apply(const BranchChannel& ob) const;

  bool Apply(std::vector<const BranchChannel*>& bch, std::vector<size_t>& id) const;
  [[deprecated]] bool Apply(const BranchChannel& a, size_t a_id, const BranchChannel& b, size_t b_id) const;

  void Init(const Configuration& conf);
  void Print() const;

  ANALYSISTREE_ATTR_NODISCARD const std::set<std::string>& GetBranches() const { return branch_names_; }
  ANALYSISTREE_ATTR_DEPRECATED()
  ANALYSISTREE_ATTR_NODISCARD const std::string&
  GetBranchName() const {
    assert(branch_names_.size() == 1);
    return *branch_names_.begin();
  }

  ANALYSISTREE_ATTR_NODISCARD std::set<size_t> GetBranchIds() const { return branch_ids_; }
  ANALYSISTREE_ATTR_NODISCARD const std::string& GetName() const { return name_; }

  std::vector<SimpleCut>& GetCuts() { return cuts_; }

  friend bool operator==(const Cuts& that, const Cuts& other);
  static bool Equal(const Cuts* that, const Cuts* other);

 protected:
  std::string name_;
  std::set<std::string> branch_names_{};
  std::set<size_t> branch_ids_{};
  std::vector<SimpleCut> cuts_{};

  bool is_init_{false};

  ClassDef(Cuts, 1);
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_CUTS_H
