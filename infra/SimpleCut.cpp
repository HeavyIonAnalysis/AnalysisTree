#include "SimpleCut.hpp"

ClassImp(AnalysisTree::SimpleCut)

namespace AnalysisTree{

void SimpleCut::Print() const {
  std::cout << title_ << std::endl;
}

void SimpleCut::FillBranchNames() {
  for (const auto &v : vars_) {
    const auto& br = v.GetBranches();
    branch_names_.insert(br.begin(), br.end());
  }
}

bool operator==(const SimpleCut &that, const SimpleCut &other) {
  if (&that == &other) {
    return true;
  }
  return that.vars_ == other.vars_ && that.title_ == other.title_;
}

}