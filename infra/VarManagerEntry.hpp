#ifndef ANALYSISTREE_INFRA_VARMANAGERENTRY_HPP_
#define ANALYSISTREE_INFRA_VARMANAGERENTRY_HPP_

#include <utility>

#include "Variable.hpp"
#include "Cuts.hpp"
#include "BranchReader.hpp"
#include "Matching.hpp"

namespace AnalysisTree{

class VarManagerEntry {

  typedef std::vector<std::vector<double>> array2D;

 public:
  VarManagerEntry() = default;

  explicit VarManagerEntry(std::vector<Variable> vars, Cuts* cuts=nullptr) :
    vars_(std::move(vars)),
    cuts_(cuts)
  {
    FillBranchNames();
  };

  void Init(const Configuration& conf, std::map<std::string, void *> &pointers_map);
  void FillValues();

  size_t AddVariable(const Variable& var);

  [[nodiscard]] const array2D& GetValues() const {return values_;}
  [[nodiscard]] const std::set<std::string>& GetBranchNames() const { return branch_names_; }
  [[nodiscard]] const std::vector<BranchReader*>& GetBranches() const { return branches_; }
  [[nodiscard]] size_t GetNumberOfBranches() const { return branch_names_.size(); }
  [[nodiscard]] const Cuts* GetCuts() const { return cuts_;}
  [[nodiscard]] const std::vector<Variable>& GetVariables() const { return vars_;}
  [[nodiscard]] std::vector<Variable>& Variables() { return vars_;}

  void AddBranchPointer(BranchReader* branch) { branches_.emplace_back(branch); }
  void SetMatching(Matching* matching) { matching_ = matching; }
  void SetIsInvertedMatching(bool is_inverted_matching) { is_inverted_matching_ = is_inverted_matching; }
  void FillBranchNames();

 private:

//  void FillVarEntry(const std::vector<int>& ch);
  void FillFromOneBranch();
  void FillFromTwoBranches();
  void FillMatchingForEventHeader(BranchReader* br1, BranchReader* br2);

  std::vector<Variable> vars_{};
  Cuts* cuts_{nullptr}; ///< non-owning

  std::set<std::string> branch_names_{};
  std::vector<BranchReader*> branches_{}; ///< non-owning pointers

  Matching* matching_{nullptr}; ///< non-owning
  bool is_inverted_matching_{false};

  array2D values_{};  ///< channels<variables>
};


}
#endif //ANALYSISTREE_INFRA_VARMANAGERENTRY_HPP_
