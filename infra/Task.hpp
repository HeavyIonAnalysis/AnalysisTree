/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_INFRA_TASK_HPP_
#define ANALYSISTREE_INFRA_TASK_HPP_

#include <set>

#include "Configuration.hpp"
#include "Constants.hpp"
#include "EventHeader.hpp"

#include "Chain.hpp"
#include "Cuts.hpp"

class TChain;
class TTree;
class TFile;

namespace AnalysisTree {

class Configuration;
class DataHeader;

class Task {

 public:
  Task() = default;
  virtual ~Task() = default;

  virtual void Init() = 0;
  virtual void Exec() = 0;
  virtual void Finish() = 0;

  void PreInit();

  void SetInConfiguration(const Configuration* config) { config_ = config; }
  void SetDataHeader(const DataHeader* data_header) { data_header_ = data_header; }

  void SetInputBranchNames(const std::set<std::string>& br) { in_branches_ = br; }

  ANALYSISTREE_ATTR_NODISCARD const std::set<std::string>& GetInputBranchNames() const { return in_branches_; }

  ANALYSISTREE_ATTR_NODISCARD bool IsGoodEvent(const EventHeader& event_header) const {
    return event_cuts_ == nullptr || event_cuts_->Apply(event_header);
  }

  ANALYSISTREE_ATTR_NODISCARD bool IsGoodEvent(const Chain& t) const;

  void SetEventCuts(Cuts* cuts) {
    if (cuts->GetBranches().size() != 1) {
      throw std::runtime_error("Event cuts on only 1 branch are allowed at the moment!");
    }
    event_cuts_ = cuts;
  }

  void AddInputBranch(const std::string& name) { in_branches_.emplace(name); }

 protected:
  const Configuration* config_{nullptr};
  const DataHeader* data_header_{nullptr};

  Cuts* event_cuts_{nullptr};

  std::set<std::string> in_branches_{};
  bool is_init_{false};

  ClassDef(Task, 0);
};

}// namespace AnalysisTree

#endif//ANALYSISTREE_INFRA_TASK_HPP_
