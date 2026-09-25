//
// Created by oleksii on 09.04.25.
//

#ifndef ANALYSISTREE_GENERICCONTAINERFILLER_HPP
#define ANALYSISTREE_GENERICCONTAINERFILLER_HPP

#include "Configuration.hpp"
#include "Container.hpp"
#include "Detector.hpp"

#include <TChain.h>
#include <TFile.h>

#include <string>
#include <vector>

namespace AnalysisTree {

struct IndexMap {
  std::string name_;
  std::string field_type_;
  short index_;
};

/// Buffer for the value of one leaf of the input tree.
/// Only the member matching the leaf type is connected via SetBranchAddress,
/// the others keep their placeholder value -199.
struct LeafBuffer {
  double double_{-199.};
  float float_{-199.f};
  int int_{-199};
  char char_{static_cast<char>(-199)};
  short short_{static_cast<short>(-199)};

  double get() const {
    if (std::fabs(double_ + 199.) > 1e-4) return double_;
    if (std::fabs(float_ + 199.f) > 1e-4) return float_;
    if (int_ != -199) return static_cast<double>(int_);
    if (char_ != static_cast<char>(-199)) return static_cast<double>(char_);
    if (short_ != static_cast<short>(-199)) return static_cast<double>(short_);
    throw std::runtime_error("GenericContainerFiller, LeafBuffer::get(): none of values initialized");
  }
};

class GenericContainerFiller {
 public:
  GenericContainerFiller() = delete;
  explicit GenericContainerFiller(std::string fileInName, std::string treeInName = "pTree");
  virtual ~GenericContainerFiller() = default;

  void SetOutputFileName(const std::string& name) { file_out_name_ = name; }
  void SetOutputTreeName(const std::string& name) { tree_out_name_ = name; }
  void SetOutputBranchName(const std::string& name) { branch_out_name_ = name; }

  void SetFieldsToIgnore(const std::vector<std::string>& fields) { fields_to_ignore_ = fields; }
  void SetFieldsToPreserve(const std::vector<std::string>& fields) { fields_to_preserve_ = fields; }

  void SetEntrySwitchTriggerVarName(const std::string& name) { entry_switch_trigger_var_name_ = name; }

  void SetNChannelsPerEntry(int n) { n_channels_per_entry_ = n; }

  void Run(int nEntries = -1);

 protected:
  void Init();
  int Exec(int iEntry, int previousTriggerVar);
  void Finish();

  static int DetermineFieldIdByName(const std::vector<IndexMap>& iMap, const std::string& name);
  void SetLeafAddress(const std::string& branchName, const IndexMap& imap, LeafBuffer& buffer);
  static void SetFieldsFromLeaves(const std::vector<IndexMap>& imap, AnalysisTree::Container& container, const std::vector<LeafBuffer>& buffers);

  std::string file_in_name_;
  std::string tree_in_name_;

  std::string file_out_name_{"AnalysisTree.root"};
  std::string tree_out_name_{"aTree"};
  std::string branch_out_name_{"PlainBranch"};

  TChain* tree_in_{nullptr};
  TFile* file_out_{nullptr};
  TTree* tree_out_{nullptr};

  AnalysisTree::Configuration config_;
  AnalysisTree::GenericDetector* generic_detector_{nullptr};
  std::vector<IndexMap> branch_map_;
  std::vector<LeafBuffer> branch_values_;

  // variable, change of value of which triggers switch to a new AT event
  std::string entry_switch_trigger_var_name_;

  int entry_switch_trigger_id_{-1};

  // if entry_switch_trigger_var_name_ is not empty, this field does not matter
  // if entry_switch_trigger_var_name_ is empty, sets how many AT channels
  // will constitute a single AT entry (event)
  int n_channels_per_entry_{-1};

  std::vector<std::string> fields_to_ignore_{};
  std::vector<std::string> fields_to_preserve_{};
};
}// namespace AnalysisTree
#endif//ANALYSISTREE_GENERICCONTAINERFILLER_HPP
