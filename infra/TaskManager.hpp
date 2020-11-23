#ifndef ANALYSISTREE_INFRA_TASKMANANGERNEW_HPP_
#define ANALYSISTREE_INFRA_TASKMANANGERNEW_HPP_

#include <string>
#include <utility>
#include <vector>

#include "Chain.hpp"
#include "Cuts.hpp"
#include "Task.hpp"
#include "Matching.hpp"

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

  void AddTask(Task* task) { tasks_.emplace_back(task); }

  template<class Branch>
  void AddBranch(const std::string& name, Branch** ptr, BranchConfig config, eBranchWriteMode mode = eBranchWriteMode::kNone) {
    if(!*ptr){
      *ptr = new Branch(configuration_->GetLastId());
    }
//    (*ptr)->Init(config);
    if(mode == eBranchWriteMode::kUpdateCurrentTree){
      chain_->Branch(name.c_str(), ptr);
      configuration_->AddBranchConfig(std::move(config));
      update_current_tree_ = true;
    }
    else if (mode == eBranchWriteMode::kCreateNewTree){
      assert(out_tree_);
      out_tree_->Branch(name.c_str(), ptr);
      configuration_->AddBranchConfig(std::move(config));
      fill_out_tree_ = true;
    }
  }

  void AddMatching(const std::string& br1, const std::string& br2, Matching** match, eBranchWriteMode mode = eBranchWriteMode::kNone){
    if(mode == eBranchWriteMode::kUpdateCurrentTree){
//      chain_->GetConfiguration()->AddMatch(match);
//      chain_->Branch(chain_->GetConfiguration()->GetMatchName(br1, br2).c_str(), &match);
//      update_current_tree_ = true;
    }
    else if (mode == eBranchWriteMode::kCreateNewTree){
      assert(out_tree_);
      *match = new Matching(configuration_->GetBranchConfig(br1).GetId(), configuration_->GetBranchConfig(br2).GetId());
      configuration_->AddMatch(*match);
      out_tree_->Branch(configuration_->GetMatchName(br1, br2).c_str(), match);
      fill_out_tree_ = true;
    }
  }


  ANALYSISTREE_ATTR_NODISCARD const Configuration* GetConfig() const { return chain_->GetConfiguration(); }
  ANALYSISTREE_ATTR_NODISCARD const DataHeader* GetDataHeader() const { return chain_->GetDataHeader(); }
  ANALYSISTREE_ATTR_NODISCARD Chain* GetChain() const { return chain_; }

  void SetOutputDataHeader(DataHeader* dh) {  data_header_ = dh; }
  void FillOutput() { out_tree_->Fill(); }

 protected:
  TaskManager() = default;
  static TaskManager* manager_;

  void InitOutChain();

//  std::unique_ptr<Chain> chain_{nullptr};
  Chain* chain_;
  std::vector<Task*> tasks_{};

  // output data members
  TTree* out_tree_{nullptr};
  Configuration* configuration_{nullptr};
  DataHeader* data_header_{nullptr};

  bool is_init_{false};
  bool fill_out_tree_{false};
  bool update_current_tree_{false};
};

};// namespace AnalysisTree

#endif//ANALYSISTREE_INFRA_TASKMANANGERNEW_HPP_
