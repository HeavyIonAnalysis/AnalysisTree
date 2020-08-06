#ifndef ANALYSISTREEQA_SRC_VARMANAGER_H_
#define ANALYSISTREEQA_SRC_VARMANAGER_H_

#include "vector"

#include "BranchReader.hpp"
#include "FillTask.hpp"
#include "Variable.hpp"
#include "Matching.hpp"
#include "VarManagerEntry.hpp"

namespace AnalysisTree {

class VarManager : public FillTask {
  typedef std::vector<std::vector<double>> array2D;

 public:
  VarManager() = default;

  std::pair<int, std::vector<int>>  AddEntry(const VarManagerEntry& vars);

  void Init(std::map<std::string, void *> &pointers_map) override;
  void Exec() override;
  void Finish() override{};

  void SetCutsMap(std::map<std::string, Cuts*> map) { cuts_map_ = std::move(map); }

  [[nodiscard]] const array2D &GetValues(int i_var) const { return vars_.at(i_var).GetValues(); }
  [[nodiscard]] const std::vector<BranchReader> &GetBranches() const { return branches_; }

  std::vector<BranchReader> &Branches() { return branches_; }
  BranchReader* GetBranch(const std::string &name);

  void FillBranchNames();

 private:
  std::vector<VarManagerEntry> vars_{};

  std::vector<BranchReader> branches_{};
  std::vector<Matching *> matching_{};
  std::map<std::string, Cuts *> cuts_map_{};
};

}// namespace AnalysisTree
#endif//ANALYSISTREEQA_SRC_VARMANAGER_H_
