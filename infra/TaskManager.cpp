#include "TaskManager.hpp"

#include <chrono>

namespace AnalysisTree {

void TaskManager::Init() {

  std::cout << "AnalysisTree::Manager::Init" << std::endl;
  auto start = std::chrono::system_clock::now();

  std::set<std::string> branch_names{};
  for (auto* task : tasks_) {
    auto br = task->GetInputBranchNames();
    branch_names.insert(br.begin(), br.end());
  }
  if (in_tree_ && in_config_) {
    branches_map_ = AnalysisTree::GetPointersToBranches(in_tree_, *in_config_, branch_names);
  }
  if (!out_file_name_.empty()) {
    out_file_ = TFile::Open(out_file_name_.c_str(), "RECREATE");
    out_file_->cd();
    if (!out_tree_name_.empty()) {
      out_tree_ = new TTree(out_tree_name_.c_str(), "Analysis Tree");
      out_config_ = new Configuration;
    }
  }

  if (event_cuts_) {
    event_cuts_->Init(*in_config_);
  }

  for (auto* task : tasks_) {
    task->SetInChain(in_tree_);
    task->SetInConfiguration(in_config_);
    task->SetDataHeader(data_header_);
    task->SetOutTree(out_tree_);
    task->SetOutFile(out_file_);
    task->SetOutConfiguration(out_config_);
    task->Init(branches_map_);
  }
  if (out_config_) {
    out_config_->Print();
    out_config_->Write("Configuration");
  }

  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "Init elapsed time: " << elapsed_seconds.count() << " s\n";
}

void TaskManager::Run(long long nEvents) {

  std::cout << "AnalysisTree::Manager::Run" << std::endl;
  auto start = std::chrono::system_clock::now();

  nEvents = nEvents < 0 ? in_tree_->GetEntries() : nEvents;

  for (long long iEvent = 0; iEvent < nEvents; ++iEvent) {
    in_tree_->GetEntry(iEvent);

    if (event_cuts_) {
      auto it = branches_map_.find(event_cuts_->GetBranchName());
      if (it == branches_map_.end()) {
        throw std::runtime_error("EventHeader " + event_cuts_->GetBranchName() + " is not found to apply event cuts");
      }
      bool is_good_event = event_cuts_->Apply(*((EventHeader*) (it->second)));
      if (!is_good_event) {
        continue;
      }
    }
    //    if ((iEvent + 1) % 100 == 0) {
    //      std::cout << "Event # " << iEvent + 1 << " out of " << nEvents << "\r" << std::flush;
    //    }
    for (auto* task : tasks_) {
      task->Exec();
    }
    if (out_tree_) {
      out_tree_->Fill();
    }
  }// Event loop

  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "elapsed time: " << elapsed_seconds.count() << ", per event: " << elapsed_seconds.count() / nEvents << "s\n";
}

void TaskManager::Finish() {

  if (out_file_)
    out_file_->cd();

  for (auto* task : tasks_) {
    task->Finish();
  }

  if (out_tree_)
    out_tree_->Write();
  if (out_file_)
    out_file_->Close();
}

}// namespace AnalysisTree