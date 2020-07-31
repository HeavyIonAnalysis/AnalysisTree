#include "Cuts.hpp"
#include "Configuration.hpp"

namespace AnalysisTree{

void Cuts::Init(const AnalysisTree::Configuration& conf) {
  for (auto &cut : cuts_) {
    for (auto &var : cut.Variables()) {
      var.Init(conf);
    }
  }

  for (const auto &v : cuts_) {
    const auto& br = v.GetBranches();
    branch_names_.insert(br.begin(), br.end());
  }
//  std::sort(branch_names_.begin(), branch_names_.end());
//  auto ip = std::unique(branch_names_.begin(), branch_names_.end());
//  branch_names_.resize(std::distance(branch_names_.begin(), ip));

  is_init_ = true;
}

void Cuts::Print() const {
  std::cout << "Cut " << name_ << " defined as:" << std::endl;
  for (const auto &cut : cuts_) {
    cut.Print();
  }
}

}