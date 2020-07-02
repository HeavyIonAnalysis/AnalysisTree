#ifndef ANALYSISTREEQA_SRC_VARMANAGER_H_
#define ANALYSISTREEQA_SRC_VARMANAGER_H_

#include <utility>

#include "vector"

#include "BranchReader.hpp"
#include "FillTask.hpp"
#include "Variable.hpp"

namespace AnalysisTree {

class VarManager : public FillTask {

 public:
  VarManager() = default;
  explicit VarManager(const std::vector<Variable> &vars) {
    CopyUniqueVars(vars);
    FillBranchNames();
    FillOutputIndexes(vars);
  }

  void Init(std::map<std::string, void *> &pointers_map) override;
  void Exec() override;
  void Finish() override{};

  void SetCutsMap(std::map<std::string, Cuts *> map) { cuts_map_ = std::move(map); }

  const std::vector<double> &GetValues(int i_var) const {
    const auto &indexes = var_indexes_.at(i_var);
    return branches_.at(indexes.first).GetValues(indexes.second);
  }

  std::vector<BranchReader> &Branches() { return branches_; }
  const BranchReader &GetBranch(const std::string &name) {
    for (const auto &branch : branches_) {
      if (branch.GetName() == name) {
        return branch;
      }
    }
    throw std::runtime_error("Branch " + name + " is not found");
  }

  const std::vector<BranchReader> &GetBranches() const { return branches_; }

 private:
  void FillOutputIndexes(const std::vector<Variable> &vars);
  void CopyUniqueVars(const std::vector<Variable> &vars);
  void FillBranchNames();
  void CreateMapUnique(const std::vector<Variable> &vars);

  std::vector<Variable> vars_{};
  std::vector<BranchReader> branches_{};
  std::vector<Matching *> matching_{};
  std::map<std::string, Cuts *> cuts_map_{};

  std::map<int, int> unique_map_{};
  std::vector<std::pair<int, int>> var_indexes_{};
};

}// namespace AnalysisTree
#endif//ANALYSISTREEQA_SRC_VARMANAGER_H_
