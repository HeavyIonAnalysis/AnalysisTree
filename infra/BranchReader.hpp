/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREEQA_SRC_BRANCHREADER_H_
#define ANALYSISTREEQA_SRC_BRANCHREADER_H_

#include <string>
#include <utility>
#include <vector>

#include "Constants.hpp"
#include "Detector.hpp"
#include "EventHeader.hpp"
#include "Utils.hpp"
#include "Variable.hpp"
#include "VariantMagic.hpp"

namespace AnalysisTree {

class Variable;
class Cuts;

/**
 * @brief BranchReader keeps data-object associated with tree branch and list of cuts for this branch
 * lots of visitor boilerplate
 */
class BranchReader {

 public:
  BranchReader() = default;
  BranchReader(std::string name, BranchPointer data, DetType type, Cuts* cuts = nullptr);
  virtual ~BranchReader() = default;

  ANALYSISTREE_ATTR_NODISCARD const std::string& GetName() const { return name_; }
  ANALYSISTREE_ATTR_NODISCARD DetType GetType() const { return type_; }
  ANALYSISTREE_ATTR_NODISCARD const Cuts* GetCut() const { return cuts_; }
  ANALYSISTREE_ATTR_NODISCARD double GetValue(const Variable& var, int i_channel) const;

  ANALYSISTREE_ATTR_NODISCARD size_t GetNumberOfChannels() const;
  ANALYSISTREE_ATTR_NODISCARD bool ApplyCut(int i_channel) const;

  ANALYSISTREE_ATTR_NODISCARD const BranchPointer& GetData() const {
    return data_;
  }

  ANALYSISTREE_ATTR_NODISCARD int GetId() const { return id_; }

 protected:
  std::string name_;
  BranchPointer data_{};
  Cuts* cuts_{nullptr};
  int id_{-1};
  DetType type_{DetType(-1)};

  ClassDef(BranchReader, 1);
};

}// namespace AnalysisTree

#endif//ANALYSISTREEQA_SRC_BRANCHREADER_H_
