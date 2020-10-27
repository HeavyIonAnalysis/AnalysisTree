#include "TaskManagerNew.hpp"

namespace AnalysisTree{

TaskManagerNew* TaskManagerNew::manager_= nullptr;

TaskManagerNew *TaskManagerNew::GetInstance()
{
  /**
   * This is a safer way to create an instance. instance = new Singleton is
   * dangeruous in case two instance threads wants to access at the same time
   */
  if(manager_==nullptr){
    manager_ = new TaskManagerNew;
  }
  return manager_;
}

void TaskManagerNew::Init(const std::vector<std::string>& filelists, const std::vector<std::string>& in_trees) {
  assert(!is_init_);
  is_init_ = true;

  in_tree_ = AnalysisTree::MakeChain(filelists, in_trees);
  in_config_ = GetConfigurationFromFileList(filelists, in_config_name_);

  try {
    data_header_ = GetObjectFromFileList<DataHeader>(filelists[0], data_header_name_);
  } catch (std::runtime_error& error) {
    std::cout << error.what() << std::endl;
  }

  for(auto* task : tasks_) {
    task->Init();
  }
}

void TaskManagerNew::Run(long long nEvents){

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

void TaskManagerNew::Finish() {
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

}