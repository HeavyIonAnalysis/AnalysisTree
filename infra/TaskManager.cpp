#include "TaskManager.hpp"

namespace AnalysisTree{

TaskManager* TaskManager::manager_= nullptr;

TaskManager* TaskManager::GetInstance()
{
  /**
   * This is a safer way to create an instance. instance = new Singleton is
   * dangeruous in case two instance threads wants to access at the same time
   */
  if(manager_==nullptr){
    manager_ = new TaskManager;
  }
  return manager_;
}

void TaskManager::Init(const std::vector<std::string>& filelists, const std::vector<std::string>& in_trees) {
  assert(!is_init_);
  is_init_ = true;

  chain_ = new Chain(filelists, in_trees);

  std::set<std::string> branch_names{};
  for (auto* task : tasks_) {
    const auto& br = task->GetInputBranchNames();
    branch_names.insert(br.begin(), br.end());
  }
  chain_->InitPointersToBranches(branch_names);

  for(auto* task : tasks_) {
    task->PreInit();
    task->Init();
  }
}

void TaskManager::Init(){

  chain_ = new Chain("aTree");

  for(auto* task : tasks_) {
    task->PreInit();
    task->Init();
  }
}

void TaskManager::Run(long long nEvents){

  std::cout << "AnalysisTree::Manager::Run" << std::endl;
  auto start = std::chrono::system_clock::now();
  nEvents = nEvents < 0 || nEvents > chain_->GetEntries() ? chain_->GetEntries() : nEvents;

  for (long long iEvent = 0; iEvent < nEvents; ++iEvent) {
    chain_->GetEntry(iEvent);
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

  delete manager_;
}

}