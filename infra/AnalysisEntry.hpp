#ifndef ANALYSISTREE_INFRA_ANALYSISENTRY_HPP_
#define ANALYSISTREE_INFRA_ANALYSISENTRY_HPP_

#include <utility>

#include "BranchReader.hpp"
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

  explicit AnalysisEntry(std::vector<Variable> vars, Cuts* cuts = nullptr) : vars_(std::move(vars)),
                                                                               cuts_(cuts) {
    FillBranchNames();
  };

  void Init(const Configuration& conf, const std::map<std::string, Matching*>& matches);
  void FillValues();

  size_t AddVariable(const Variable& var);

  ANALYSISTREE_ATTR_NODISCARD const array2D& GetValues() const { return values_; }
  ANALYSISTREE_ATTR_NODISCARD const std::set<std::string>& GetBranchNames() const { return branch_names_; }
  ANALYSISTREE_ATTR_NODISCARD const std::vector<BranchReader>& GetBranches() const { return branches_; }
  ANALYSISTREE_ATTR_NODISCARD size_t GetNumberOfBranches() const { return branch_names_.size(); }
  ANALYSISTREE_ATTR_NODISCARD const Cuts* GetCuts() const { return cuts_; }
  ANALYSISTREE_ATTR_NODISCARD const std::vector<Variable>& GetVariables() const { return vars_; }
  ANALYSISTREE_ATTR_NODISCARD std::vector<Variable>& Variables() { return vars_; }

  void AddBranchPointer(const BranchReader& branch) { branches_.emplace_back(branch); }
  void SetMatching(Matching* matching) { matching_ = matching; }
  void SetIsInvertedMatching(bool is_inverted_matching) { is_inverted_matching_ = is_inverted_matching; }
  void FillBranchNames();

 private:
  //  void FillVarEntry(const std::vector<int>& ch);
  void FillFromOneBranch();
  void FillFromTwoBranches();
  void FillMatchingForEventHeader(const BranchReader& br1, const BranchReader& br2);
  ANALYSISTREE_ATTR_NODISCARD bool ApplyCutOnBranch(const BranchReader& br, int i_channel) const;
  ANALYSISTREE_ATTR_NODISCARD bool ApplyCutOnBranches(const BranchReader& br1, int ch1, const BranchReader& br2, int ch2) const;
  static double FillVariabe(const Variable& var, const BranchReader& br1, int ch1, const BranchReader& br2, int ch2);

  std::vector<Variable> vars_{};
  Cuts* cuts_{nullptr};///< non-owning

  std::set<std::string> branch_names_{};
  std::vector<BranchReader> branches_{};///< non-owning pointers

  Matching* matching_{nullptr};///< non-owning
  bool is_inverted_matching_{false};

  array2D values_{};///< channels<variables>
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_INFRA_ANALYSISENTRY_HPP_
