/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_INFRA_ANALYSISENTRY_HPP_
#define ANALYSISTREE_INFRA_ANALYSISENTRY_HPP_

#include <utility>

#include "Branch.hpp"
#include "Cuts.hpp"
#include "Matching.hpp"
#include "Variable.hpp"

namespace AnalysisTree {

/**
 * @brief AnalysisEntry keeps list of Variables from one or more branches.
 * Before each event AnalysisEntry evaluates Variables and Cuts
 */
class AnalysisEntry {

  typedef std::vector<std::vector<double>> array2D;

 public:
  AnalysisEntry() = default;
  virtual ~AnalysisEntry();

  explicit AnalysisEntry(std::vector<Variable> vars, Cuts* cuts = nullptr) : vars_(std::move(vars)),
                                                                             cuts_(cuts) {
    FillBranchNames();
  };

  void Init(const Configuration& conf, const std::map<std::string, Matching*>& matches);
  void FillValues();

  size_t AddVariable(const Variable& var);

  ANALYSISTREE_ATTR_NODISCARD const array2D& GetValues() const { return values_; }
  ANALYSISTREE_ATTR_NODISCARD const std::set<std::string>& GetBranchNames() const { return branch_names_; }
  //  ANALYSISTREE_ATTR_NODISCARD const std::vector<Branch>& GetBranches() const { return branches_; }
  ANALYSISTREE_ATTR_NODISCARD size_t GetNumberOfBranches() const { return branch_names_.size(); }
  ANALYSISTREE_ATTR_NODISCARD const Cuts* GetCuts() const { return cuts_; }
  ANALYSISTREE_ATTR_NODISCARD const std::vector<Variable>& GetVariables() const { return vars_; }
  ANALYSISTREE_ATTR_NODISCARD std::vector<Variable>& Variables() { return vars_; }

  void AddBranch(const Branch& branch, Cuts* cuts = nullptr);
  void SetMatching(Matching* matching) { matching_ = matching; }
  void SetIsInvertedMatching(bool is_inverted_matching) { is_inverted_matching_ = is_inverted_matching; }
  void FillBranchNames();

 private:
  void FillFromEveHeaders();
  void FillFromOneChannalizedBranch();
  void FillFromTwoChannalizedBranches();
  ANALYSISTREE_ATTR_NODISCARD bool ApplyCutOnBranch(const Branch& br, Cuts* cuts, int i_channel) const;
  ANALYSISTREE_ATTR_NODISCARD bool ApplyCutOnBranches(std::vector<const Branch*>& br, std::vector<Cuts*>& cuts, std::vector<int>& ch) const;
  [[deprecated]] ANALYSISTREE_ATTR_NODISCARD bool ApplyCutOnBranches(const Branch& br1, Cuts* cuts1, int ch1, const Branch& br2, Cuts* cuts2, int ch2) const;
  static double FillVariable(const Variable& var, std::vector<const Branch*>& br, std::vector<int>& id);
  [[deprecated]] static double FillVariable(const Variable& var, const Branch& br1, int ch1, const Branch& br2, int ch2);

  std::vector<Variable> vars_{};
  Cuts* cuts_{nullptr};///< non-owning

  std::set<std::string> branch_names_{};
  std::vector<std::pair<const Branch*, Cuts*>> branches_{};///< non-owning pointers

  std::vector<int> eve_header_indices_{};
  std::vector<int> non_eve_header_indices_{};

  Matching* matching_{nullptr};///< non-owning
  bool is_inverted_matching_{false};

  array2D values_{};///< channels<variables>

  ClassDef(AnalysisEntry, 1);
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_INFRA_ANALYSISENTRY_HPP_
