#ifndef ANALYSISTREE_INFRA_VARMANAGERENTRY_HPP_
#define ANALYSISTREE_INFRA_VARMANAGERENTRY_HPP_

#include <utility>

#include "BranchReader.hpp"
#include "Cuts.hpp"
#include "Matching.hpp"
#include "Variable.hpp"

namespace AnalysisTree::Version1 {

/**
 * @brief VarManager entry keeps list of Variables from one or more branches.
 * Before each event VarManagerEntry evaluates Variables and Cuts
 */
class VarManagerEntry {

  typedef std::vector<std::vector<double>> array2D;

 public:
  VarManagerEntry() = default;

  explicit VarManagerEntry(std::vector<Variable> vars, Cuts* cuts = nullptr) : vars_(std::move(vars)),
                                                                               cuts_(cuts) {
    FillBranchNames();
  };

  void Init(const AnalysisTree::Configuration& conf, std::map<std::string, void*>& pointers_map);
  void FillValues();

  size_t AddVariable(const Variable& var);

  ANALYSISTREE_ATTR_NODISCARD const array2D& GetValues() const { return values_; }
  ANALYSISTREE_ATTR_NODISCARD const std::set<std::string>& GetBranchNames() const { return branch_names_; }
  ANALYSISTREE_ATTR_NODISCARD const std::vector<BranchReader*>& GetBranches() const { return branches_; }
  ANALYSISTREE_ATTR_NODISCARD size_t GetNumberOfBranches() const { return branch_names_.size(); }
  ANALYSISTREE_ATTR_NODISCARD const Cuts* GetCuts() const { return cuts_; }
  ANALYSISTREE_ATTR_NODISCARD const std::vector<Variable>& GetVariables() const { return vars_; }
  ANALYSISTREE_ATTR_NODISCARD std::vector<Variable>& Variables() { return vars_; }

  void AddBranchPointer(BranchReader* branch) { branches_.emplace_back(branch); }
  void SetMatching(Matching* matching) { matching_ = matching; }
  void SetIsInvertedMatching(bool is_inverted_matching) { is_inverted_matching_ = is_inverted_matching; }
  void FillBranchNames();

 private:
  //  void FillVarEntry(const std::vector<int>& ch);
  void FillFromOneBranch();
  void FillFromTwoBranches();
  void FillMatchingForEventHeader(BranchReader* br1, BranchReader* br2);
  bool ApplyCutOnBranch(BranchReader* br, int i_channel) const;
  bool ApplyCutOnBranches(BranchReader* br1, int ch1, BranchReader* br2, int ch2) const;
  static double FillVariabe(const Variable& var, BranchReader* br1, int ch1, BranchReader* br2, int ch2);

  std::vector<Variable> vars_{};
  Cuts* cuts_{nullptr};///< non-owning

  std::set<std::string> branch_names_{};
  std::vector<BranchReader*> branches_{};///< non-owning pointers

  Matching* matching_{nullptr};///< non-owning
  bool is_inverted_matching_{false};

  array2D values_{};///< channels<variables>
};

}// namespace AnalysisTree::Version1
#endif//ANALYSISTREE_INFRA_VARMANAGERENTRY_HPP_
