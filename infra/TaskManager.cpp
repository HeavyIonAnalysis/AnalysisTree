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
  assert(!is_init_);
  is_init_ = true;

  InitOutChain();
  configuration_ = new Configuration("Configuration");
//  data_header_ = new DataHeader;
  chain_ = new Chain(out_tree_, configuration_, nullptr);

  for(auto* task : tasks_) {
    task->PreInit();
    task->Init();
  }
}

void TaskManager::InitOutChain(){
  out_tree_ = new TTree("aTree", "");
}

void TaskManager::Run(long long nEvents){

  std::cout << "AnalysisTree::Manager::Run" << std::endl;
  auto start = std::chrono::system_clock::now();

  if(chain_->GetEntries() > 0){
    nEvents = nEvents < 0 || nEvents > chain_->GetEntries() ? chain_->GetEntries() : nEvents;
  }

  for (long long iEvent = 0; iEvent < nEvents; ++iEvent) {
    chain_->GetEntry(iEvent);
    for (auto* task : tasks_) {
      task->Exec();
    }
    if (fill_out_tree_) {
      out_tree_->Fill();
    }
  }// Event loop

  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "elapsed time: " << elapsed_seconds.count() << ", per event: " << elapsed_seconds.count() / nEvents << "s\n";
}

void TaskManager::Finish() {
  for (auto* task : tasks_) {
    task->Finish();
  }
  if (fill_out_tree_){
    TFile *f = TFile::Open("test.root", "recreate");
    out_tree_->Write();
    f->Close();
  }

  delete manager_;
}

}