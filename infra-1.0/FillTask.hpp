#ifndef ANALYSISTREE_FILLTASK_H_
#define ANALYSISTREE_FILLTASK_H_

// core
#include <AnalysisTree/Configuration.hpp>
// infra
#include "Utils.hpp"

class TChain;
class TTree;
class TFile;

namespace AnalysisTree{
class Configuration;
class DataHeader;
}

namespace AnalysisTree::Version1 {

class FillTask {

 public:
  FillTask() = default;
  virtual ~FillTask() = default;

  virtual void Init(std::map<std::string, void*>&) = 0;
  virtual void Exec() = 0;
  virtual void Finish() = 0;

  void SetInChain(TChain* in_chain);

  void SetInConfiguration(AnalysisTree::Configuration* config) {
    config_ = config;
  }

  void SetOutTree(TTree* out_tree) { out_tree_ = out_tree; }
  void SetOutConfiguration(AnalysisTree::Configuration* config) { out_config_ = config; }
  void SetDataHeader(AnalysisTree::DataHeader* data_header) { data_header_ = data_header; }
  void SetOutFile(TFile* f) { out_file_ = f; }
  void SetInputBranchNames(const std::vector<std::string>& br) { in_branches_ = br; }
  void SetOutputBranchName(const std::string& br) { out_branch_ = br; }

  ANALYSISTREE_ATTR_NODISCARD const std::string& GetOutputBranchName() const { return out_branch_; }

  ANALYSISTREE_ATTR_NODISCARD const std::vector<std::string>& GetInputBranchNames() const { return in_branches_; }

 protected:
  TChain* in_chain_{nullptr};
  AnalysisTree::Configuration* config_{nullptr};
  AnalysisTree::DataHeader* data_header_{nullptr};

  TFile* out_file_{nullptr};
  TTree* out_tree_{nullptr};
  AnalysisTree::Configuration* out_config_{nullptr};

  std::vector<std::string> in_branches_{};
  std::string out_branch_{};
};

}// namespace AnalysisTree::Version1

#endif//ANALYSISTREE_FILLTASK_H_
