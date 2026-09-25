/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_PLAINTREEFILLER_H_
#define ANALYSISTREE_PLAINTREEFILLER_H_

#include <utility>

#include "AnalysisTask.hpp"
#include "Detector.hpp"
#include "Variable.hpp"

namespace AnalysisTree {

struct FIB {
  double double_{-299.};
  float float_{-299.f};
  int int_{-299};
  bool bool_{false};
  Types type_{Types::kNumberOfTypes};
};

class PlainTreeFiller : public AnalysisTask {
 public:
  PlainTreeFiller() = default;

  void AddBranch(const std::string& branch_name);

  void Init() override;
  void Exec() override;
  void Finish() override;

  void SetOutputName(std::string file, std::string tree) {
    file_name_ = std::move(file);
    tree_name_ = std::move(tree);
  }

  void SetFieldsToIgnore(const std::vector<std::string>& fields_to_ignore);
  void SetFieldsToPreserve(const std::vector<std::string>& fields_to_preserve);
  void SetFieldsToRename(const std::vector<std::pair<std::string, std::string>>& fields_to_rename);

  void SetIsIgnoreDefaultFields(bool is = true) { is_ignore_defual_fields_ = is; }
  void SetIsPrependLeavesWithBranchName(bool is = true) { is_prepend_leaves_with_branchname_ = is; }

 protected:
  void CheckIgnorePreserveRenameFields(const std::vector<std::string>& leafNames) const;

  TFile* file_{nullptr};
  TTree* plain_tree_{nullptr};

  std::string file_name_{"PlainTree.root"};
  std::string tree_name_{"PlainTree"};
  std::string branch_name_;

  std::vector<FIB> vars_;
  std::vector<std::string> fields_to_ignore_{};
  std::vector<std::string> fields_to_preserve_{};
  std::map<std::string, std::string> fields_to_rename_{};

  bool is_ignore_defual_fields_{false};
  bool is_prepend_leaves_with_branchname_{true};
};

}// namespace AnalysisTree

#endif//ANALYSISTREE_PLAINTREEFILLER_H_
