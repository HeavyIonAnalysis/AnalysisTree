/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "TaskManager.hpp"

#include <iostream>

namespace AnalysisTree {

TaskManager* TaskManager::manager_ = nullptr;

TaskManager* TaskManager::GetInstance() {
  /**
   * This is a safer way to create an instance. instance = new Singleton is
   * dangerous in case two instance threads wants to access at the same time
   */
  if (manager_ == nullptr) {
    manager_ = new TaskManager;
  }
  return manager_;
}

void TaskManager::Init(const std::vector<std::string>& filelists, const std::vector<std::string>& in_trees) {
  assert(!is_init_);
  std::cout << "TaskManager::Init()\n";
  is_init_ = true;
  read_in_tree_ = true;
  chain_ = new Chain(filelists, in_trees);

  std::set<std::string> branch_names{};
  for (auto* task : tasks_) {
    const auto& br = task->GetInputBranchNames();
    branch_names.insert(br.begin(), br.end());
  }
  chain_->InitPointersToBranches(branch_names);

  if (fill_out_tree_) {
    InitOutChain();
  }

  PrintCommitInfo();

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
  std::cout << "TaskManager::Init()\n";
  is_init_ = true;
  fill_out_tree_ = true;

  InitOutChain();
  chain_ = new Chain(out_tree_, configuration_, data_header_);

  InitTasks();
}

void TaskManager::InitOutChain() {
  assert(fill_out_tree_);
  out_file_ = TFile::Open(out_file_name_.c_str(), "recreate");
  configuration_ = new Configuration("Configuration");
  data_header_ = new DataHeader;

  if (write_mode_ == eBranchWriteMode::kCreateNewTree) {
    out_tree_ = new TTree(out_tree_name_.c_str(), "AnalysisTree");
  } else if (write_mode_ == eBranchWriteMode::kCopyTree) {
    assert(configuration_ && data_header_ && chain_);// input should exist
    configuration_ = chain_->CloneConfiguration();
    *(data_header_) = *(chain_->GetDataHeader());
    for (const auto& brex : branches_exclude_) {
      if (chain_->CheckBranchExistence(brex) == 1) {
        throw std::runtime_error("AnalysisTree::TaskManager::InitOutChain - Tree in the input file does not support selective cloning");
      }
      chain_->SetBranchStatus((brex + ".*").c_str(), false);
      for (const auto& maex : configuration_->GetMatchesOfBranch(brex)) {
        chain_->SetBranchStatus((maex + ".*").c_str(), false);
      }
      configuration_->RemoveBranchConfig(brex);
    }
    out_tree_ = chain_->CloneChain(0);
    out_tree_->SetName(out_tree_name_.c_str());
    data_header_ = chain_->GetDataHeader();
    chain_->SetBranchStatus("*", true);
  }
  out_tree_->SetAutoSave(0);
}

void TaskManager::Run(long long nEvents) {
  if (chain_->GetEntries() > 0) {
    nEvents = nEvents < 0 || nEvents > chain_->GetEntries() ? chain_->GetEntries() : nEvents;
  }
  Run(0, nEvents);
}

void TaskManager::Run(long long nEventFrom, long long nEvents) {
  std::cout << "AnalysisTree::Manager::Run" << std::endl;
  auto start = std::chrono::system_clock::now();

  if (nEventFrom + nEvents > chain_->GetEntries() && chain_->GetEntries() > 0) {
    throw std::runtime_error("TaskManager::Run() - nEventFrom + nEvents > chain_->GetEntries()");
  }

  if (verbosity_frequency_ > 0) {
    const int verbosityPeriod = nEvents / verbosity_frequency_;
    const int vPlog = static_cast<int>(std::round(std::log10(verbosityPeriod)));
    verbosity_period_ = static_cast<int>(std::pow(10, vPlog));
  }

  for (long long iEvent = nEventFrom; iEvent < nEventFrom + nEvents; ++iEvent) {
    if (verbosity_period_ > 0 && iEvent % verbosity_period_ == 0) {
      std::cout << "Event no " << iEvent << "\n";
    }
    if (read_in_tree_) {
      chain_->GetEntry(iEvent);
    }
    Exec();
  }// Event loop

  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "elapsed time: " << elapsed_seconds.count() << ", per event: " << elapsed_seconds.count() / nEvents << "s\n";
}

void TaskManager::WriteCommitInfo() {
  std::string tag = std::getenv("ANALYSIS_TREE_TAG") ? std::getenv("ANALYSIS_TREE_TAG") : "unknown";
  std::string commit = std::getenv("ANALYSIS_TREE_COMMIT_HASH") ? std::getenv("ANALYSIS_TREE_COMMIT_HASH") : "unknown";
  std::string is_original = std::getenv("ANALYSIS_TREE_COMMIT_ORIGINAL") ? std::getenv("ANALYSIS_TREE_COMMIT_ORIGINAL") : "unknown";
  TNamed("AnalysisTree_tag", tag).Write();
  TNamed("AnalysisTree_commit_hash", commit).Write();
  TNamed("AnalysisTree_commit_is_original", is_original).Write();
}

void TaskManager::PrintCommitInfo() {
  std::string tag = std::getenv("ANALYSIS_TREE_TAG") ? std::getenv("ANALYSIS_TREE_TAG") : "unknown";
  std::string commit = std::getenv("ANALYSIS_TREE_COMMIT_HASH") ? std::getenv("ANALYSIS_TREE_COMMIT_HASH") : "unknown";
  std::string is_original = std::getenv("ANALYSIS_TREE_COMMIT_ORIGINAL") ? std::getenv("ANALYSIS_TREE_COMMIT_ORIGINAL") : "unknown";
  std::cout << "ANALYSIS_TREE_TAG = " << tag << "\n"
            << "ANALYSIS_TREE_COMMIT_HASH = " << commit << "\n"
            << "ANALYSIS_TREE_COMMIT_ORIGINAL = " << is_original << "\n";
}

void TaskManager::Finish() {

  for (auto* task : tasks_) {
    task->Finish();
  }

  if (fill_out_tree_) {
    std::cout << "Output file is " << out_file_name_ << std::endl;
    std::cout << "Output tree is " << out_tree_name_ << std::endl;
    out_file_->cd();
    if (out_tree_->GetListOfFriends() != nullptr) {
      if (out_tree_->GetListOfFriends()->GetEntries() != 0) {
        std::cout << "Warining: TaskManager::Finish() - out_tree_ has friends which can be wrongly read from the output file\n";
      }
    }
    out_tree_->Write();
    configuration_->Write("Configuration");
    data_header_->Write("DataHeader");
    if (is_write_hash_info_) WriteCommitInfo();
    out_file_->Close();
    out_tree_ = nullptr;
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
  if (is_owns_tasks_) {
    for (auto* task_ptr : tasks_) {
      std::cout << "removing task" << std::endl;
      delete task_ptr;
    }
  }
  ClearTasks();
}
void TaskManager::Exec() {
  for (auto* task : tasks_) {
    if (!task->IsGoodEvent(*chain_)) continue;
    task->Exec();
  }
  if (fill_out_tree_ && is_update_entry_in_exec_) {
    FillOutput();
  }
}
}// namespace AnalysisTree
