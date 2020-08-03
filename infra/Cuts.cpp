#include "Cuts.hpp"
#include "Configuration.hpp"

namespace AnalysisTree{

void Cuts::Init(const AnalysisTree::Configuration& conf) {
  for (auto &cut : cuts_) {
    for (auto &var : cut.Variables()) {
      var.Init(conf);
    }
  }

  for(const auto& branch : branch_names_){
    branch_ids_.insert(conf.GetBranchConfig(branch).GetId());
  }

  is_init_ = true;
}

void Cuts::Print() const {
  std::cout << "Cut " << name_ << " defined as:" << std::endl;
  for (const auto &cut : cuts_) {
    cut.Print();
  }
}

}