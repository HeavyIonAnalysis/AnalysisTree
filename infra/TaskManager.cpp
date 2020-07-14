#include "TaskManager.hpp"

#include "TTree.h"
#include "TFile.h"
#include "TChain.h"

namespace AnalysisTree {

void TaskManager::Init() {

  std::cout << "AnalysisTree::Manager::Init" << std::endl;

  std::vector<std::string> branch_names{};
  for (auto *task : tasks_) {
    for (const auto &branch : task->GetInputBranchNames())
      branch_names.emplace_back(branch);
  }
  if(in_tree_ && in_config_){
    branches_map_ = AnalysisTree::GetPointersToBranches(in_tree_, *in_config_, branch_names);
  }
  if (!out_file_name_.empty()) {
    out_file_ = TFile::Open(out_file_name_.c_str(), "RECREATE");
    out_file_->cd();
    if (!out_tree_name_.empty()) {
      out_tree_ = new TTree(out_tree_name_.c_str(), "Analysis Tree");
    }
  }

  for (auto *task : tasks_) {
    task->SetInChain(in_tree_);
    task->SetInConfiguration(in_config_);
    task->SetDataHeader(data_header_);
    task->SetOutTree(out_tree_);
    task->SetOutConfiguration(out_config_);
    task->Init(branches_map_);
  }
  if (out_config_) {
    out_config_->Print();
    out_config_->Write("Configuration");
  }
}

void TaskManager::Run(long long nEvents) {

  std::cout << "AnalysisTree::Manager::Run" << std::endl;
  nEvents = nEvents < 0 ? in_tree_->GetEntries() : nEvents;

  for (long long iEvent = 0; iEvent < nEvents; ++iEvent) {
    in_tree_->GetEntry(iEvent);
//    if ((iEvent + 1) % 100 == 0) {
//      std::cout << "Event # " << iEvent + 1 << " out of " << nEvents << "\r" << std::flush;
//    }
    for (auto *task : tasks_) {
      task->Exec();
    }
    if (out_tree_) {
      out_tree_->Fill();
    }
  }// Event loop
}

void TaskManager::Finish() {

  if (out_file_)
    out_file_->cd();

  for (auto *task : tasks_) {
    task->Finish();
  }

  if (out_tree_)
    out_tree_->Write();
  if (out_file_)
    out_file_->Close();
}

}// namespace AnalysisTree