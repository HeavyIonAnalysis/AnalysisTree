#include "TTree.h"

#include "PlainTreeFiller.hpp"

namespace AnalysisTree {

void PlainTreeFiller::AddBranch(const std::string& branch_name) {
  branch_name_ = branch_name;
  in_branches_.emplace_back(branch_name);
}

void PlainTreeFiller::Init(std::map<std::string, void*>& branches) {

//  config_->Print();

  if(!branch_name_.empty()){
    const auto& branch_config = config_->GetBranchConfig(branch_name_);
    for (const auto& field : branch_config.GetMap<float>()) {
      VarManager::AddEntry(VarManagerEntry({Variable(branch_name_, field.first)}));
    }
    for (const auto& field : branch_config.GetMap<int>()) {
      VarManager::AddEntry(VarManagerEntry({Variable(branch_name_, field.first)}));
    }
    for (const auto& field : branch_config.GetMap<bool>()) {
      VarManager::AddEntry(VarManagerEntry({Variable(branch_name_, field.first)}));
    }
  }

  VarManager::Init(branches);
  if(entries_.size() != 1){
    throw std::runtime_error("Only 1 output branch");
  }
  const auto& vars = entries_[0].GetVariables();
  vars_.resize(vars.size());

  plain_tree_ = new TTree(tree_name_.c_str(), "Plain Tree");
  for (size_t i=0; i<vars.size(); ++i) {
    std::string leaf_name = vars[i].GetName();
    plain_tree_->Branch(vars[i].GetName().c_str(), &(vars_.at(i)), Form("%s/F", leaf_name.c_str()));
  }
}

void PlainTreeFiller::Exec() {
  VarManager::Exec();
  const auto& values = entries_[0].GetValues();
  for(const auto& channel : values){
    assert (channel.size() == vars_.size());
    for (size_t i=0; i<channel.size(); ++i) {
      vars_.at(i) = channel.at(i);
    }
    plain_tree_->Fill();
  }
}

void PlainTreeFiller::Finish() {
  VarManager::Finish();
  plain_tree_->Write();
}
}// namespace AnalysisTree