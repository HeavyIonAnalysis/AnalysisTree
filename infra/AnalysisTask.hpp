#ifndef ANALYSISTREE_INFRA_ANALYSISTASK_HPP_
#define ANALYSISTREE_INFRA_ANALYSISTASK_HPP_

#include "vector"

#include "BranchReader.hpp"
#include "Task.hpp"
#include "Matching.hpp"
#include "VarManagerEntry.hpp"
#include "Variable.hpp"

namespace AnalysisTree {

/**
 * @brief AnalysisTask is a Task which keeps list of VarManagerEntry-s
 * It evaluates Variables according to the rules of VarManagerEntry-s and
 * populates 2-dimensional array with results of evaluation
 */
class AnalysisTask : public Task {
  typedef std::vector<std::vector<double>> array2D;

 public:
  AnalysisTask() = default;

  std::pair<int, std::vector<int>> AddEntry(const VarManagerEntry& vars);

  void Init() override;
  void Exec() override;
  void Finish() override{};

  [[nodiscard]] const array2D& GetValues(int i_var) const { return entries_.at(i_var).GetValues(); }
  [[nodiscard]] const std::vector<BranchReader>& GetBranches() const { return branches_; }

  std::vector<BranchReader>& Branches() { return branches_; }
  BranchReader GetBranch(const std::string& name);

  void FillBranchNames();

  [[nodiscard]] const std::vector<VarManagerEntry>& GetVarEntries() const { return entries_; }
  [[nodiscard]] std::vector<VarManagerEntry>& VarEntries() { return entries_; }

 protected:
  std::vector<VarManagerEntry> entries_{};
  std::vector<BranchReader> branches_{};
  std::map<std::string, Cuts*> cuts_map_{};
};

}// namespace AnalysisTree

#endif//ANALYSISTREE_INFRA_ANALYSISTASK_HPP_
