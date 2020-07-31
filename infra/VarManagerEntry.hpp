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

  explicit VarManagerEntry(std::vector<Variable>  vars, Cuts* cuts=nullptr) :
    vars_(std::move(vars)),
    cuts_(cuts)
  {};

  void Init(const Configuration& conf);

  void FillValues();

  const array2D& GetValues() const {return values_;}

  const std::set<std::string>& GetBranches() const { return branch_names_; }
  void AddBranchPointer(BranchReader* branch) { branches_.emplace_back(branch); }
  void SetMatching(Matching* matching) { matching_ = matching; }
  void SetIsInvertedMatching(bool is_inverted_matching) { is_inverted_matching_ = is_inverted_matching; }
  void FillBranchNames();


 private:

  void FillFromOneBranch();
  void FillFromTwoBranches();
  void FillMatchingForEventHeader(BranchReader* br1, BranchReader* br2);

  std::vector<Variable> vars_{};
  Cuts* cuts_{nullptr}; ///< non-owning
  std::set<std::string> branch_names_{};
  std::vector<BranchReader*> branches_{}; ///< non-owning pointers
  Matching* matching_{nullptr}; ///< non-owning
  bool is_inverted_matching_{false};

  array2D values_{};
};


}
#endif //ANALYSISTREE_INFRA_VARMANAGERENTRY_HPP_
