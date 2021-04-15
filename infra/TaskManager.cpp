#include "TaskManager.hpp"

#include <chrono>

namespace AnalysisTree {

struct TaskManager::EventCuts {
  std::vector<std::pair<Cuts, EventHeader*>> factorized_cuts;

  bool EvalCuts() const {
    if (factorized_cuts.empty()) {
      return true;
    }
    for (auto &cuts_pair : factorized_cuts) {
      if (!cuts_pair.first.Apply(*cuts_pair.second)) {
        return false;
      }
    }
    return true;
  }
};

void TaskManager::Init() {

  std::cout << "AnalysisTree::Manager::Init" << std::endl;
  auto start = std::chrono::system_clock::now();

  std::set<std::string> branch_names{};
  for (auto *task : tasks_) {
    auto br = task->GetInputBranchNames();
    branch_names.insert(br.begin(), br.end());
  }

  if (event_cuts_) {
    branch_names.insert(event_cuts_->GetBranches().cbegin(), event_cuts_->GetBranches().cend());
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

  /* Here we will split AnalysisTree::Cuts to the parts with one branch per part */
  if (event_cuts_) {
    event_cuts_new_ = new EventCuts;
    std::map<std::string, AnalysisTree::Cuts> single_branch_cuts;
    for (auto &simple_cut : event_cuts_->GetCuts()) {
      assert(simple_cut.GetBranches().size() == 1);
      auto simple_cut_branch_name = *(simple_cut.GetBranches().begin());
      assert(in_config_->GetBranchConfig(simple_cut_branch_name).GetType() == DetType::kEventHeader);
      auto emplace_result = single_branch_cuts.emplace(simple_cut_branch_name, Cuts());
      auto &cuts = emplace_result.first->second;
      cuts.GetCuts().emplace_back(simple_cut);
    }
    for (auto &single_branch_cut_entry : single_branch_cuts) {
      auto &branch_name = single_branch_cut_entry.first;
      auto &cuts = single_branch_cut_entry.second;
      cuts.Init(*in_config_);
      auto data_ptr = (EventHeader *) branches_map_.at(single_branch_cut_entry.first);
      event_cuts_new_->factorized_cuts.emplace_back(std::make_pair(cuts, data_ptr));
    }
//    event_cuts_->Init(*in_config_);
  }

  for (auto *task : tasks_) {
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

  nEvents = nEvents < 0 || nEvents > in_tree_->GetEntries() ? in_tree_->GetEntries() : nEvents;

  for (long long iEvent = 0; iEvent < nEvents; ++iEvent) {
    in_tree_->GetEntry(iEvent);

    if (event_cuts_new_) {
      if (!event_cuts_new_->EvalCuts()) {
        continue;
      }
    }
    for (auto *task : tasks_) {
      task->Exec();
    }
    if (out_tree_) {
      out_tree_->Fill();
    }
  }// Event loop

  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "elapsed time: " << elapsed_seconds.count() << ", per event: " << elapsed_seconds.count() / nEvents
            << "s\n";
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