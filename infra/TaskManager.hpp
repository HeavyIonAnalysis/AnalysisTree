#ifndef ANALYSISTREE_INFRA_TASKMANANGERNEW_HPP_
#define ANALYSISTREE_INFRA_TASKMANANGERNEW_HPP_

#include <string>
#include <utility>
#include <vector>

#include "Chain.hpp"
#include "Cuts.hpp"
#include "Task.hpp"
//#include "TreeReader.hpp"

class TTree;
class TFile;
class TChain;

namespace AnalysisTree {

class Configuration;

class TaskManager {

 public:

  enum class eBranchWriteMode{
    kUpdateCurrentTree = 0,
    kCreateNewTree,
    kNone
  };

  static TaskManager* GetInstance();

  TaskManager(TaskManager&other) = delete;
  void operator=(const TaskManager&) = delete;
  virtual ~TaskManager() = default;

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

  void SetOutputName(std::string file, std::string tree) {
    out_file_name_ = std::move(file);
    out_tree_name_ = std::move(tree);
  }

  void AddTask(Task* task) { tasks_.emplace_back(task); }

  template<class Branch>
  void AddBranch(const std::string& name, Branch* ptr, eBranchWriteMode mode = eBranchWriteMode::kNone) {
    if(mode == eBranchWriteMode::kUpdateCurrentTree){
      chain_->Branch(name.c_str(), &ptr);
      update_current_tree_ = true;
    }
    else if (mode == eBranchWriteMode::kCreateNewTree){
      assert(out_tree_);
      out_tree_->Branch(name.c_str(), &ptr);
      fill_out_tree_ = true;
    }
//    branches_map_.emplace(name, ptr);
  }

  [[nodiscard]] const Configuration* GetConfig() const { return chain_->GetConfiguration(); }
  [[nodiscard]] const DataHeader* GetDataHeader() const { return chain_->GetDataHeader(); }
  [[nodiscard]] Chain* GetChain() const { return chain_; }

 protected:
  TaskManager() = default;
  static TaskManager* manager_;

//  std::unique_ptr<Chain> chain_{nullptr};
  Chain* chain_;
  std::vector<Task*> tasks_{};

  // output data members
  std::string out_file_name_;
  std::string out_tree_name_;
  TFile* out_file_{nullptr};
  TTree* out_tree_{nullptr};
  AnalysisTree::Configuration* out_config_{nullptr};

  bool is_init_{false};

  bool fill_out_tree_{false};
  bool update_current_tree_{false};
};

};// namespace AnalysisTree

#endif//ANALYSISTREE_INFRA_TASKMANANGERNEW_HPP_
