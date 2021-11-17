#include "SimpleCut.hpp"

#include <iostream>

namespace AnalysisTree::Version1 {

void SimpleCut::Print() const {
  std::cout << title_ << std::endl;
}

void SimpleCut::FillBranchNames() {
  for (const auto& v : vars_) {
    const auto& br = v.GetBranches();
    branch_names_.insert(br.begin(), br.end());
  }
}

bool operator==(const SimpleCut& that, const SimpleCut& other) {
  if (&that == &other) {
    return true;
  }
  return that.vars_ == other.vars_ && that.title_ == other.title_;
}

SimpleCut RangeCut(const std::string& variable_name, float lo, float hi, const std::string& title) {
  return SimpleCut(Variable::FromString(variable_name), lo, hi, title);
}
SimpleCut EqualsCut(const std::string& variable_name, float value, const std::string& title) {
  return SimpleCut(Variable::FromString(variable_name), value, title);
}
}// namespace AnalysisTree::Version1

ClassImp(AnalysisTree::Version1::SimpleCut)
