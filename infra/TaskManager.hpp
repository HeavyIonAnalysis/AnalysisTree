#ifndef ANALYSISTREE_INFRA_TASKMANAGER_H_
#define ANALYSISTREE_INFRA_TASKMANAGER_H_

#include <cstdlib>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

#include "Detector.hpp"
#include "EventHeader.hpp"
#include "Track.hpp"

#include "Cuts.hpp"
#include "FillTask.hpp"
#include "TreeReader.hpp"

namespace AnalysisTree {

class TaskManager {

 public:
  TaskManager() = delete;
  //  TaskManager(const std::string& filelist, const std::string& in_tree) :
  //    in_tree_(MakeChain(filelist, in_tree)),
  //    in_config_(GetObjectFromFileList<Configuration>(filelist, in_config_name_))
  //  {
  //    try{
  //      data_header_ = GetObjectFromFileList<DataHeader>(filelist, data_header_name_);
  //    }
  //    catch(std::runtime_error&) {}
  //  }

  TaskManager(const std::vector<std::string> &filelists,
              const std::vector<std::string> &in_trees) : in_tree_(AnalysisTree::MakeChain(filelists, in_trees)),
                                                          in_config_(GetObjectFromFileList<Configuration>(filelists[0], in_config_name_)) {
    try {
      data_header_ = GetObjectFromFileList<DataHeader>(filelists[0], data_header_name_);
    } catch (std::runtime_error &error) {
      std::cout << error.what() << std::endl;
    }
  }

  virtual void Init();
  virtual void Run(long long nEvents);
  virtual void Finish();

  void AddTask(FillTask *task) { tasks_.emplace_back(task); }

  virtual ~TaskManager() = default;

  void SetOutFileName(std::string name) {
    out_file_name_ = std::move(name);
  }

  void SetEventCuts(Cuts *cuts) { event_cuts_ = cuts; }

  void AddBranchCut(Cuts *cuts) {
    cuts_map_.insert(std::make_pair(cuts->GetBranchName(), cuts));
  }

 protected:
  TFile *out_file_{nullptr};
  TTree *out_tree_{nullptr};

  TChain *in_tree_{nullptr};
  std::string data_header_name_{"DataHeader"};
  std::string in_config_name_{"Configuration"};
  std::string out_file_name_{""};
  std::string out_tree_name_{""};

  AnalysisTree::Configuration *out_config_{nullptr};
  AnalysisTree::Configuration *in_config_{nullptr};
  AnalysisTree::DataHeader *data_header_{nullptr};
  Cuts *event_cuts_{nullptr};

  std::vector<FillTask *> tasks_{};

  std::map<std::string, void *> branches_map_{};
  std::map<std::string, Cuts *> cuts_map_{};
};

};// namespace AnalysisTree

#endif//ANALYSISTREE_INFRA_TASKMANAGER_H_
