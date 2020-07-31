#include "VarManagerEntry.hpp"

namespace AnalysisTree{

void VarManagerEntry::FillFromOneBranch(){
  BranchReader* br = branches_.at(0);
  const auto n_channels = br->GetNumberOfChannels();
  values_.reserve(n_channels);

  for (size_t i_channel = 0; i_channel < n_channels; ++i_channel) {
    if (!br->ApplyCut(i_channel)) continue;
    if (cuts_ && !std::visit([this, i_channel](auto &&arg) { return cuts_->Apply(arg->GetChannel(i_channel)); }, br->GetData())) continue;
    std::vector<double> temp_vars(vars_.size());
    short i_var{0};
    for (const auto &var : vars_) {
      temp_vars[i_var] = std::visit([var, i_channel](auto &&arg) { return var.GetValue(arg->GetChannel(i_channel)); }, br->GetData());
      i_var++;
    }//variables
    values_.emplace_back(temp_vars);
  }  //channels
}

void VarManagerEntry::FillMatchingForEventHeader(BranchReader* br1, BranchReader* br2){
  matching_ = new Matching(br1->GetId(), br2->GetId());
  for(size_t i=0; i<br1->GetNumberOfChannels(); ++i){
    matching_->AddMatch(i, 0);
  }
}

void VarManagerEntry::FillFromTwoBranches(){
  BranchReader* br1 = branches_.at(0);
  BranchReader* br2 = branches_.at(1);
  if(br1->GetType() == DetType::kEventHeader) { //put EventHeader to second place, to be able to fill matching
    std::swap(br1, br2);
  }
  if(br2->GetType() == DetType::kEventHeader){
    FillMatchingForEventHeader(br1, br2);
  }
  values_.reserve(matching_->GetMatches().size());

  for (auto match : matching_->GetMatches(is_inverted_matching_)) {
    const int ch1 = match.first;
    const int ch2 = match.second;
    if (!br1->ApplyCut(ch1)) continue;
    if (!br2->ApplyCut(ch2)) continue;
    auto cut_func = [this, ch1, ch2](auto &&arg1, auto &&arg2) { return cuts_->Apply(arg1->GetChannel(ch1), arg1->GetId(), arg2->GetChannel(ch2), arg2->GetId()); };
    if (cuts_ && !std::visit(cut_func, br1->GetData(), br2->GetData())) continue;

    std::vector<double> temp_vars(vars_.size());
    short i_var{};
    for (const auto &var : vars_) {
      auto func = [var, ch1, ch2](auto &&arg1, auto &&arg2) {return var.GetValue(arg1->GetChannel(ch1), arg1->GetId(), arg2->GetChannel(ch2), arg2->GetId()); };
      temp_vars[i_var] = std::visit(func, br1->GetData(), br2->GetData());
      i_var++;
    }//variables
    values_.emplace_back(temp_vars);
  }

}

void VarManagerEntry::FillValues() {
  values_.clear();
  if(branches_.size()==1){
    FillFromOneBranch();
  }
  else if(branches_.size()==2){
    FillFromTwoBranches();
  }
  else {
    throw std::runtime_error("VarManagerEntry::FillValues - branches_.size() is more than 2 or 0");
  }
}

void VarManagerEntry::FillBranchNames() {
  for (auto &var : vars_) {
    const auto& br = var.GetBranches();
    branch_names_.insert(br.begin(), br.end());
  }
  if(cuts_){
    const auto& br = cuts_->GetBranches();
    branch_names_.insert(br.begin(), br.end());
  }
}

void VarManagerEntry::Init(const AnalysisTree::Configuration& conf) {
  if(cuts_){
    cuts_->Init(conf);
  }
  for (auto &var : vars_) {
    var.Init(conf);
  }
}

}