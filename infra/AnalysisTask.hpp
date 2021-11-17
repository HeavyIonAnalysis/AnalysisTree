/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_INFRA_ANALYSISTASK_HPP_
#define ANALYSISTREE_INFRA_ANALYSISTASK_HPP_

#include "vector"

#include "AnalysisEntry.hpp"
#include "Matching.hpp"
#include "Task.hpp"
#include "Variable.hpp"

namespace AnalysisTree {

/**
 * @brief AnalysisTask is a Task which keeps list of AnalysisEntry-s
 * It evaluates Variables according to the rules of AnalysisEntry-s and
 * populates 2-dimensional array with results of evaluation
 */
class AnalysisTask : public Task {
  typedef std::vector<std::vector<double>> array2D;

 public:
  AnalysisTask() = default;
  ~AnalysisTask() override = default;

  std::pair<int, std::vector<int>> AddEntry(const AnalysisEntry& vars);

  void Init() override;
  void Exec() override;
  void Finish() override{};

  ANALYSISTREE_ATTR_NODISCARD const array2D& GetValues(int i_var) const { return entries_.at(i_var).GetValues(); }
  ANALYSISTREE_ATTR_NODISCARD const std::vector<AnalysisEntry>& GetVarEntries() const { return entries_; }
  ANALYSISTREE_ATTR_NODISCARD std::vector<AnalysisEntry>& VarEntries() { return entries_; }

  void AddBranchCut(Cuts* cut) {
    if (cut->GetBranches().size() != 1) {
      throw std::runtime_error("Branch cuts on only 1 branch are allowed at the moment!");
    }
    std::string br_name = *(cut->GetBranches().begin());
    if (cuts_map_.find(br_name) != cuts_map_.end()) {
      throw std::runtime_error("Branch cut on " + br_name + " already exists");
    }
    cuts_map_[br_name] = cut;
  }

 protected:
  std::vector<AnalysisEntry> entries_{};
  std::map<std::string, Cuts*> cuts_map_{};

  ClassDefOverride(AnalysisTask, 0);
};

}// namespace AnalysisTree

#endif//ANALYSISTREE_INFRA_ANALYSISTASK_HPP_
