#ifndef ANALYSISTREE_INFRA_TASKMANANGERNEW_HPP_
#define ANALYSISTREE_INFRA_TASKMANANGERNEW_HPP_

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

class TaskManager {

 public:
  enum class eBranchWriteMode {
    kCreateNewTree,
    kNone
  };

  static TaskManager* GetInstance();

  TaskManager(TaskManager& other) = delete;
  void operator=(const TaskManager&) = delete;
  virtual ~TaskManager();

  /**
   * @brief If TaskManager owns its tasks, they will be deleted on TaskManager' destruction
   * @param owns_flag
   */
  void SetOwnsTasks(bool owns_flag = true) { is_owns_tasks = owns_flag; }

  /**
 * Initialization in case of reading AnalysisTree
 * @param filelists vector of filelists -> text files with paths to all root files
 * @param in_trees vector ot TTree names
 */
  virtual void Init(const std::vector<std::string>& filelists, const std::vector<std::string>& in_trees);

  /**
  * Initialization in case of writing AnalysisTree
  */
  virtual void Init();
  virtual void Run(long long nEvents);
  virtual void Finish();

  void AddTask(Task* task) { tasks_.emplace_back(task); }

  /**
* Adding a new branch
* @param name name of the branch
* @param ptr reference to a pointer to the branch object. Pointer shoulb be initialized with nullprt, function will allocate the space, but used still needs delete it in the end of the program
* @param mode write or not the branch to the file
*/
  template<class Branch>
  void AddBranch(const std::string& name, Branch*& ptr, BranchConfig config, eBranchWriteMode mode = eBranchWriteMode::kCreateNewTree) {
    assert(!name.empty() && !ptr);

    if (mode == eBranchWriteMode::kCreateNewTree) {
      assert(out_tree_);
      fill_out_tree_ = true;

      configuration_->AddBranchConfig(std::move(config));
      ptr = new Branch(configuration_->GetLastId());
      out_tree_->Branch(name.c_str(), &ptr);// otherwise I get segfault at filling in case of large number of Particles
    } else {
      throw std::runtime_error("Not yet implemented...");
    }
  }

  /**
* Adding a new Matching branch
* @param br1 name of the first branch
* @param br2 name of the second branch
* @param match reference to a pointer to the Matching object. Pointer shoulb be initialized with nullprt, function will allocate the space, but used still needs delete it in the end of the program
* @param mode write or not the branch to the file
*/
  void AddMatching(const std::string& br1, const std::string& br2, Matching*& match, eBranchWriteMode mode = eBranchWriteMode::kCreateNewTree) {
    assert(!br1.empty() && !br2.empty() && !match);
    match = new Matching(configuration_->GetBranchConfig(br1).GetId(),
                         configuration_->GetBranchConfig(br2).GetId());

    if (mode == eBranchWriteMode::kCreateNewTree) {
      assert(out_tree_);
      configuration_->AddMatch(match);
      out_tree_->Branch(configuration_->GetMatchName(br1, br2).c_str(), &match);
      fill_out_tree_ = true;
    } else {
      throw std::runtime_error("Not yet implemented...");
    }
  }

  ANALYSISTREE_ATTR_NODISCARD const Configuration* GetConfig() const { return chain_->GetConfiguration(); }
  ANALYSISTREE_ATTR_NODISCARD const DataHeader* GetDataHeader() const { return chain_->GetDataHeader(); }
  ANALYSISTREE_ATTR_NODISCARD Chain* GetChain() const { return chain_; }

  void SetOutputDataHeader(DataHeader* dh) {
    data_header_ = dh;
    chain_->SetDataHeader(dh);// TODO
  }
  void FillOutput() { out_tree_->Fill(); }

  void Exec() {
    for (auto* task : tasks_) {
      task->Exec();
    }
    if (fill_out_tree_) {
      FillOutput();
    }
  }

  void SetOutputName(std::string file, std::string tree = "aTree") {
    out_file_name_ = std::move(file);
    out_tree_name_ = std::move(tree);
  }

 protected:
  TaskManager() = default;
  static TaskManager* manager_;

  void InitOutChain();
  void InitTasks();

  //  std::unique_ptr<Chain> chain_{nullptr};
  Chain* chain_{nullptr};
  std::vector<Task*> tasks_{};

  // output data members
  TTree* out_tree_{nullptr};
  TFile* out_file_{nullptr};
  Configuration* configuration_{nullptr};
  DataHeader* data_header_{nullptr};
  std::string out_tree_name_{"aTree"};
  std::string out_file_name_{"analysis_tree.root"};

  bool is_init_{false};
  bool fill_out_tree_{false};
  bool read_in_tree_{false};
  bool is_owns_tasks{true};

  ClassDef(TaskManager, 1);
};

};// namespace AnalysisTree

#endif//ANALYSISTREE_INFRA_TASKMANANGERNEW_HPP_
