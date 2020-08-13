#include "TTree.h"

#include "PlainTreeFiller.hpp"

namespace AnalysisTree {

void PlainTreeFiller::Init(std::map<std::string, void*>& branches) {
  in_branch_ = (AnalysisTree::Particles*) branches.find(in_branches_[0])->second;

  if (vars_.empty()) {//copy all variables
    const auto& branch_config = config_->GetBranchConfig(in_branches_[0]);

    for (const auto& field : branch_config.GetMap<float>()) {
      vars_.emplace_back(std::make_pair(0.f, Variable(in_branches_[0], field.first)));
    }
    for (const auto& field : branch_config.GetMap<int>()) {
      vars_.emplace_back(std::make_pair(0.f, Variable(in_branches_[0], field.first)));
    }
    for (const auto& field : branch_config.GetMap<bool>()) {
      vars_.emplace_back(std::make_pair(0.f, Variable(in_branches_[0], field.first)));
    }
  }

  out_tree_ = new TTree(tree_name_.c_str(), "Plain TTree from AnalysisTree");

  for (auto& var : vars_) {
    var.second.Init(*config_);
    out_tree_->Branch(var.second.GetName().c_str(), &var.first, Form("%s/F", var.second.GetName().c_str()));
  }
}

void PlainTreeFiller::Exec() {
  for (const auto& channel : *in_branch_->GetChannels()) {
    for (auto& var : vars_) {
      var.first = var.second.GetValue(channel);
    }
    out_tree_->Fill();
  }
}

}// namespace AnalysisTree