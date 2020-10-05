#ifndef ANALYSISTREEQA_SRC_VARMANAGER_H_
#define ANALYSISTREEQA_SRC_VARMANAGER_H_

#include "vector"

#include "BranchReader.hpp"
#include "FillTask.hpp"
#include "Matching.hpp"
#include "VarManagerEntry.hpp"
#include "Variable.hpp"

namespace AnalysisTree {

/**
 * @brief VarManager is a FillTask which keeps list of VarManagerEntry-s
 * It evaluates Variables according to the rules of VarManagerEntry-s and
 * populates 2-dimensional array with results of evaluation
 */
class VarManager : public FillTask {
  typedef std::vector<std::vector<double>> array2D;

 public:
  VarManager() = default;

  std::pair<int, std::vector<int>> AddEntry(const VarManagerEntry& vars);

  void Init(std::map<std::string, void*>& pointers_map) override;
  void Exec() override;
  void Finish() override{};

  void SetCutsMap(std::map<std::string, Cuts*> map) { cuts_map_ = std::move(map); }

  [[nodiscard]] const array2D& GetValues(int i_var) const { return entries_.at(i_var).GetValues(); }
  [[nodiscard]] const std::vector<BranchReader>& GetBranches() const { return branches_; }

  std::vector<BranchReader>& Branches() { return branches_; }
  BranchReader* GetBranch(const std::string& name);

  void FillBranchNames();

  [[nodiscard]] const std::vector<VarManagerEntry>& GetVarEntries() const { return entries_; }
  [[nodiscard]] std::vector<VarManagerEntry>& VarEntries() { return entries_; }

 protected:
  std::vector<VarManagerEntry> entries_{};
  std::vector<BranchReader> branches_{};
  std::vector<Matching*> matching_{};
  std::map<std::string, Cuts*> cuts_map_{};
};

}// namespace AnalysisTree
#endif//ANALYSISTREEQA_SRC_VARMANAGER_H_
