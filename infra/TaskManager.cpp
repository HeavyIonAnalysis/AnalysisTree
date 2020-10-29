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

  in_tree_ = AnalysisTree::MakeChain(filelists, in_trees);
  in_config_ = GetConfigurationFromFileList(filelists, in_config_name_);
  try {
    data_header_ = GetObjectFromFileList<DataHeader>(filelists[0], data_header_name_);
  } catch (std::runtime_error& error) {
    std::cout << error.what() << std::endl;
  }

  std::set<std::string> branch_names{};
  for (auto* task : tasks_) {
    auto br = task->GetInputBranchNames();
    branch_names.insert(br.begin(), br.end());
  }
  branches_map_ = AnalysisTree::GetPointersToBranches(in_tree_, *in_config_, branch_names);

  for(auto* task : tasks_) {
    task->PreInit();
    task->Init();
  }
}

void TaskManager::Run(long long nEvents){

  std::cout << "AnalysisTree::Manager::Run" << std::endl;
  auto start = std::chrono::system_clock::now();
  nEvents = nEvents < 0 || nEvents > in_tree_->GetEntries() ? in_tree_->GetEntries() : nEvents;

  for (long long iEvent = 0; iEvent < nEvents; ++iEvent) {
    in_tree_->GetEntry(iEvent);
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