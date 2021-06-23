/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "BranchReader.hpp"
#include "Cuts.hpp"

namespace AnalysisTree {

BranchReader::BranchReader(std::string name, BranchPointer data, DetType type, Cuts* cuts) : name_(std::move(name)),
                                                                                             data_(data),
                                                                                             cuts_(cuts),
                                                                                             type_(type) {
  id_ = ANALYSISTREE_UTILS_VISIT(get_id_struct(), data_);
}

size_t BranchReader::GetNumberOfChannels() const {
  return ANALYSISTREE_UTILS_VISIT(get_n_channels_struct(), data_);
}

bool BranchReader::ApplyCut(int i_channel) const {
  if (!cuts_) return true;
  return ANALYSISTREE_UTILS_VISIT(apply_cut(i_channel, cuts_), data_);
}

double BranchReader::GetValue(const Variable& var, int i_channel) const {
  return ANALYSISTREE_UTILS_VISIT(get_value(var, i_channel), data_);
}

}// namespace AnalysisTree