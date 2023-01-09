/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_INFRA_TASKMANANGERNEW_HPP_
#define ANALYSISTREE_INFRA_TASKMANANGERNEW_HPP_

#include <chrono>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "Chain.hpp"
#include "Cuts.hpp"
#include "Matching.hpp"
#include "Task.hpp"

class TTree;
class TFile;
class TChain;

namespace AnalysisTree {

class Configuration;

enum class eBranchWriteMode {
  kCreateNewTree,
  kCopyTree,
  kNone
};

class TaskManager {

 public:
  static TaskManager* GetInstance();

  TaskManager(TaskManager& other) = delete;
  void operator=(const TaskManager&) = delete;
  virtual ~TaskManager();

  /**
   * @brief If TaskManager owns its tasks, they will be deleted on TaskManager' destruction
   * @param owns_flag
   */
  void SetOwnsTasks(bool owns_flag = true) { is_owns_tasks_ = owns_flag; }

  /**
 * Initialization in case of reading AnalysisTree (or reading + creating)
 * @param filelists vector of filelists -> text files with paths to all root files
 * @param in_trees vector ot TTree names
 */
  virtual void Init(const std::vector<std::string>& filelists, const std::vector<std::string>& in_trees);

  /**
  * Initialization in case of only creating AnalysisTree
  */
  virtual void Init();
  virtual void Run(long long nEvents = -1);
  virtual void Finish();

  void AddTask(Task* task) { tasks_.emplace_back(task); }

  /**
* Adding a new branch
* @param name name of the branch
* @param ptr reference to a pointer to the branch object. Pointer should be initialized with nullprt, function will allocate the space, but used still needs delete it in the end of the program
*/
  template<class BranchPtr>
  void AddBranch(BranchPtr*& ptr, const BranchConfig& config) {
    if (out_tree_ == nullptr) {
      throw std::runtime_error("No output tree. Probably, TaskManager::Init() was not called or output file/tree names are not set.");
    }

    if (!ptr) {
      ptr = new BranchPtr(config.GetId());
    } else if (ptr->GetId() != config.GetId()) {
      std::cout << "WARNING! ptr->GetId() != config.GetId()!" << std::endl;
      ptr = new BranchPtr(config.GetId());
    }
    configuration_->AddBranchConfig(config);
    if (write_mode_ == eBranchWriteMode::kCreateNewTree) {
      chain_->GetConfiguration()->AddBranchConfig(config);
    }

    out_tree_->Branch((config.GetName() + ".").c_str(), &ptr);
  }

  void AddBranch(Branch* branch) {
    switch (branch->GetBranchType()) {
      case DetType::kParticle: {
        AddBranch(branch->GetDataRaw<Particles*>(), branch->GetConfig());
        break;
      }
      case DetType::kTrack: {
        AddBranch(branch->GetDataRaw<TrackDetector*>(), branch->GetConfig());
        break;
      }
      case DetType::kHit: {
        AddBranch(branch->GetDataRaw<HitDetector*>(), branch->GetConfig());
        break;
      }
      case DetType::kModule: {
        AddBranch(branch->GetDataRaw<ModuleDetector*>(), branch->GetConfig());
        break;
      }
      case DetType::kEventHeader: {
        AddBranch(branch->GetDataRaw<EventHeader*>(), branch->GetConfig());
        break;
      }
    }
  }

  /**
* Adding a new Matching branch
* @param br1 name of the first branch
* @param br2 name of the second branch
* @param match reference to a pointer to the Matching object. Pointer shoulb be initialized with nullprt, function will allocate the space, but used still needs delete it in the end of the program
*/
  void AddMatching(const std::string& br1, const std::string& br2, Matching*& match) {
    assert(!br1.empty() && !br2.empty() && !match);
    if (out_tree_ == nullptr) {
      throw std::runtime_error("No output tree. Probably, TaskManager::Init() was not called or output file/tree names are not set.");
    }

    match = new Matching(configuration_->GetBranchConfig(br1).GetId(),
                         configuration_->GetBranchConfig(br2).GetId());

    configuration_->AddMatch(match);
    //    if (write_mode_ == eBranchWriteMode::kCreateNewTree) {
    //      chain_->GetConfiguration()->AddMatch(match);
    //    }
    out_tree_->Branch((configuration_->GetMatchName(br1, br2) + ".").c_str(), &match);
  }

  ANALYSISTREE_ATTR_NODISCARD const Configuration* GetConfig() const { return chain_->GetConfiguration(); }
  ANALYSISTREE_ATTR_NODISCARD const DataHeader* GetDataHeader() const { return chain_->GetDataHeader(); }
  ANALYSISTREE_ATTR_NODISCARD Chain* GetChain() const { return chain_; }

  ANALYSISTREE_ATTR_NODISCARD const Configuration* GetOutConfig() const { return configuration_; }
  ANALYSISTREE_ATTR_NODISCARD const DataHeader* GetOutDataHeader() const { return data_header_; }

  void SetOutputDataHeader(DataHeader* dh) {
    data_header_ = dh;
    chain_->SetDataHeader(dh);// TODO
  }
  void FillOutput() { out_tree_->Fill(); }

  void Exec() {
    for (auto* task : tasks_) {
      if (!task->IsGoodEvent(*chain_)) continue;
      task->Exec();
    }
    if (fill_out_tree_) {
      FillOutput();
    }
  }

  std::vector<Task*>& Tasks() { return tasks_; }

  void SetOutputName(std::string file, std::string tree = "aTree") {
    out_file_name_ = std::move(file);
    out_tree_name_ = std::move(tree);
    fill_out_tree_ = true;
  }

  void SetWriteMode(eBranchWriteMode mode) { write_mode_ = mode; }
  void SetBranchesExclude(std::vector<std::string> brex) { branches_exclude_ = std::move(brex); }
  void SetVerbosityPeriod(int value) { verbosity_period_ = value; }

  void ClearTasks() { tasks_.clear(); }

 protected:
  TaskManager() = default;
  static TaskManager* manager_;

  void InitOutChain();
  void InitTasks();

  // input data members
  Chain* chain_{nullptr};
  std::vector<Task*> tasks_{};

  // output data members
  TFile* out_file_{nullptr};
  TTree* out_tree_{nullptr};
  Configuration* configuration_{nullptr};///< output
  DataHeader* data_header_{nullptr};     ///< output
  std::string out_tree_name_{"aTree"};
  std::string out_file_name_{"analysis_tree.root"};
  std::vector<std::string> branches_exclude_{};

  int verbosity_period_{-1};

  // configuration parameters
  eBranchWriteMode write_mode_{eBranchWriteMode::kCreateNewTree};
  bool is_init_{false};
  bool fill_out_tree_{false};
  bool read_in_tree_{false};
  bool is_owns_tasks_{true};

  ClassDef(TaskManager, 0);
};

};// namespace AnalysisTree

#endif//ANALYSISTREE_INFRA_TASKMANANGERNEW_HPP_
