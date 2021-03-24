#include "TaskManager.hpp"

#include <iostream>

namespace AnalysisTree {

TaskManager* TaskManager::manager_ = nullptr;

TaskManager* TaskManager::GetInstance() {
  /**
   * This is a safer way to create an instance. instance = new Singleton is
   * dangeruous in case two instance threads wants to access at the same time
   */
  if (manager_ == nullptr) {
    manager_ = new TaskManager;
  }
  return manager_;
}

void TaskManager::Init(const std::vector<std::string>& filelists, const std::vector<std::string>& in_trees) {
  assert(!is_init_);
  is_init_ = true;
  read_in_tree_ = true;
  chain_ = new Chain(filelists, in_trees);

  std::set<std::string> branch_names{};
  for (auto* task : tasks_) {
    const auto& br = task->GetInputBranchNames();
    branch_names.insert(br.begin(), br.end());
  }
  chain_->InitPointersToBranches(branch_names);

  if(fill_out_tree_){
    InitOutChain();
  }

  InitTasks();
}

void TaskManager::InitTasks() {
  for (auto* task : tasks_) {
    task->PreInit();
    task->Init();
  }
}

void TaskManager::Init() {
  assert(!is_init_);
  is_init_ = true;

  InitOutChain();
  chain_ = new Chain(out_tree_, configuration_, data_header_);

  InitTasks();
}

void TaskManager::InitOutChain() {
  out_file_ = TFile::Open(out_file_name_.c_str(), "recreate");
  out_tree_ = new TTree(out_tree_name_.c_str(), "AnalysisTree");
  configuration_ = new Configuration("Configuration");
  data_header_ = new DataHeader;
}

void TaskManager::Run(long long nEvents) {

  std::cout << "AnalysisTree::Manager::Run" << std::endl;
  auto start = std::chrono::system_clock::now();

  if (chain_->GetEntries() > 0) {
    nEvents = nEvents < 0 || nEvents > chain_->GetEntries() ? chain_->GetEntries() : nEvents;
  }

  for (long long iEvent = 0; iEvent < nEvents; ++iEvent) {
    if (read_in_tree_) {
      chain_->GetEntry(iEvent);
    }
    Exec();
  }// Event loop

  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "elapsed time: " << elapsed_seconds.count() << ", per event: " << elapsed_seconds.count() / nEvents << "s\n";
}

void TaskManager::Finish() {

  for (auto* task : tasks_) {
    task->Finish();
  }

  if (fill_out_tree_) {
    std::cout << "Output file is " << out_file_name_ << std::endl;
    std::cout << "Output tree is " << out_tree_name_ << std::endl;
    out_file_->cd();
    out_tree_->Write();
    configuration_->Write("Configuration");
    data_header_->Write("DataHeader");
    out_file_->Close();
    delete out_file_;
    delete configuration_;
    delete data_header_;
  }

  out_tree_name_ = "aTree";
  out_file_name_ = "analysis_tree.root";
  is_init_ = false;
  fill_out_tree_ = false;
  read_in_tree_ = false;
}

TaskManager::~TaskManager() {
  if (is_owns_tasks) {
    for (auto* task_ptr : tasks_) {
      std::cout << "removing task" << std::endl;
      delete task_ptr;
    }
  }
  tasks_.clear();
}
}// namespace AnalysisTree