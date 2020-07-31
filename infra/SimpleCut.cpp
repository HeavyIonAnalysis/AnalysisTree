#include "SimpleCut.hpp"


ClassImp(AnalysisTree::SimpleCut)

void AnalysisTree::SimpleCut::Print() const {
  std::cout << title_ << std::endl;
}

void AnalysisTree::SimpleCut::FillBranchNames() {
  for (const auto &v : vars_) {
    const auto& br = v.GetBranches();
    branch_names_.insert(br.begin(), br.end());
  }
}
