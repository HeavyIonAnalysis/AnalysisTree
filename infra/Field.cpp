/* Copyright (C) 2019-2021 GSI, Universität Tübingen, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Eugeny Kashirin, Ilya Selyuzhenkov */
#include "Field.hpp"
#include "Configuration.hpp"

#include <iostream>

namespace AnalysisTree {

void Field::Print() const {
  std::cout << "Field::Print():" << std::endl;
  std::cout << "  branch: " << branch_ << std::endl;
  std::cout << "  field: " << field_ << std::endl;

  if (is_init_) {
    std::cout << "  branch_id: " << branch_id_ << std::endl;
    std::cout << "  branch_type: " << (short) branch_type_ << std::endl;
    std::cout << "  field_id: " << field_id_ << std::endl;
    std::cout << "  type: " << (short) field_type_ << std::endl;
  }
}

bool operator==(const Field& that, const Field& other) {
  if (&that == &other) {
    return true;
  }
  return that.branch_ == other.branch_ && that.field_ == other.field_;
}

bool operator>(const Field& that, const Field& other) {
  return that.branch_ + that.field_ > other.branch_ + other.field_;
}

bool operator<(const Field& that, const Field& other) {
  return that.branch_ + that.field_ < other.branch_ + other.field_;
}

void Field::Init(const Configuration& conf) {
  const auto& branch_conf = conf.GetBranchConfig(branch_);
  Init(branch_conf);
}

void Field::Init(const BranchConfig& branch_conf) {
  branch_id_ = branch_conf.GetId();
  branch_type_ = branch_conf.GetType();
  field_id_ = branch_conf.GetFieldId(field_);
  field_type_ = branch_conf.GetFieldType(field_);
  if (field_id_ == UndefValueInt) {
    std::cout << "WARNING!! Field::Init - " << field_ << " is not found in branch " << branch_ << std::endl;
  }
  is_init_ = true;
}

}// namespace AnalysisTree
