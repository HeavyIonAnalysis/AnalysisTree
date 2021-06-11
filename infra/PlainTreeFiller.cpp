/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "TTree.h"

#include "PlainTreeFiller.hpp"

namespace AnalysisTree {

void PlainTreeFiller::AddBranch(const std::string& branch_name) {
  branch_name_ = branch_name;
  in_branches_.emplace(branch_name);
}

void PlainTreeFiller::Init() {

  if (!branch_name_.empty()) {
    const auto& branch_config = config_->GetBranchConfig(branch_name_);
    for (const auto& field : branch_config.GetMap<float>()) {
      AnalysisTask::AddEntry(AnalysisEntry({Variable(branch_name_, field.first)}));
    }
    for (const auto& field : branch_config.GetMap<int>()) {
      AnalysisTask::AddEntry(AnalysisEntry({Variable(branch_name_, field.first)}));
    }
    for (const auto& field : branch_config.GetMap<bool>()) {
      AnalysisTask::AddEntry(AnalysisEntry({Variable(branch_name_, field.first)}));
    }
  }

  AnalysisTask::Init();

  if (entries_.size() != 1) {
    throw std::runtime_error("Only 1 output branch");
  }
  const auto& vars = entries_[0].GetVariables();
  vars_.resize(vars.size());

  file_ = TFile::Open(file_name_.c_str(), "recreate");
  plain_tree_ = new TTree(tree_name_.c_str(), "Plain Tree");
  for (size_t i = 0; i < vars.size(); ++i) {
    std::string leaf_name = vars[i].GetName();
    plain_tree_->Branch(vars[i].GetName().c_str(), &(vars_.at(i)), Form("%s/F", leaf_name.c_str()));
  }
}

void PlainTreeFiller::Exec() {
  AnalysisTask::Exec();
  const auto& values = entries_[0].GetValues();
  for (const auto& channel : values) {
    assert(channel.size() == vars_.size());
    for (size_t i = 0; i < channel.size(); ++i) {
      vars_.at(i) = channel.at(i);
    }
    plain_tree_->Fill();
  }
}

void PlainTreeFiller::Finish() {
  AnalysisTask::Finish();
  file_->cd();
  plain_tree_->Write();
  file_->Close();
  delete file_;
  //  delete plain_tree_;
}
}// namespace AnalysisTree