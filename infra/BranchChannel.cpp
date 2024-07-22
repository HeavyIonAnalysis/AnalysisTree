/* Copyright (C) 2019-2021 GSI, MEPhI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Eugeny Kashirin, Viktor Klochkov, Ilya Selyuzhenkov */
#include "BranchChannel.hpp"

#include <utility>

#include "Branch.hpp"
#include "EventHeader.hpp"
#include "Field.hpp"

using namespace AnalysisTree;

void BranchChannel::UpdateChannel(size_t new_channel) {
  i_channel_ = new_channel;
  UpdatePointer();
}

void BranchChannel::UpdatePointer() {
  if (i_channel_ >= 0 && i_channel_ < branch_->size()) {
    data_ptr_ = ANALYSISTREE_UTILS_VISIT(get_channel_struct(i_channel_), branch_->GetData());
  } else {
    throw std::out_of_range("");
  }
}

void BranchChannel::CopyContent(const BranchChannel& other, std::string branch_name_prefix) {
  branch_->CheckMutable();
  auto mapping_it = branch_->copy_fields_mapping.find(other.branch_);
  if (mapping_it == branch_->copy_fields_mapping.end()) {
    branch_->CreateMapping(other.branch_, std::move(branch_name_prefix));
    mapping_it = branch_->copy_fields_mapping.find(other.branch_);
  }

  /* Eval mapping */
  const auto& field_pairs = mapping_it->second.field_pairs;

  for (auto& field_pair /* src : dst */ : field_pairs) {
    this->SetValue(field_pair.second, other.Value(field_pair.first));
  }
}

void BranchChannel::MergeContentFromTwoChannels(const BranchChannel& first, const BranchChannel& second) {
  int matching_case{-1};
  bool first_channel_active = !first.IsNullChannel();
  bool second_channel_active = !second.IsNullChannel();
  if (first_channel_active) {
    if (second_channel_active) matching_case = 0;
    else
      matching_case = 1;
  } else {
    if (second_channel_active) matching_case = 2;
    else
      throw std::runtime_error("BranchChannel::MergeContentFromTwoChannels() : both channels are null");
  }

  if (first_channel_active) CopyContent(first);
  if (second_channel_active) CopyContent(second, second.branch_->GetBranchName());
  this->SetValue(branch_->GetField("matching_case"), matching_case);
}

BranchChannel::BranchChannel(const Branch* branch, std::size_t i_channel) : branch_(branch), i_channel_(i_channel) {
  UpdatePointer();
}

void BranchChannel::Print(std::ostream& os) const {
  os << "Branch " << branch_->GetBranchName() << " channel #" << i_channel_ << std::endl;
}

double BranchChannel::Value(const Field& v) const {
  //  assert(v.GetBranchId() == branch_->GetId()); // TODO
  assert(v.IsInitialized());

  if (v.GetName() == "ones") return 1;

  using AnalysisTree::Types;
  switch (v.GetFieldType()) {
    case Types::kFloat: return ANALYSISTREE_UTILS_VISIT(get_field_struct<float>(v.GetFieldId()), data_ptr_);
    case Types::kInteger: return ANALYSISTREE_UTILS_VISIT(get_field_struct<int>(v.GetFieldId()), data_ptr_);
    case Types::kBool: return ANALYSISTREE_UTILS_VISIT(get_field_struct<bool>(v.GetFieldId()), data_ptr_);
    default: throw std::runtime_error("Field type is not correct!");//NOTE commented because of Ones field NOTE now commented again
  }
}

void BranchChannel::SetValue(const Field& v, double value) {
  using AnalysisTree::Types;
  switch (v.GetFieldType()) {
    case Types::kFloat: ANALYSISTREE_UTILS_VISIT(set_field_struct<float>(value, v.GetFieldId()), data_ptr_); break;
    case Types::kInteger: ANALYSISTREE_UTILS_VISIT(set_field_struct<int>(value, v.GetFieldId()), data_ptr_); break;
    case Types::kBool: ANALYSISTREE_UTILS_VISIT(set_field_struct<bool>(value, v.GetFieldId()), data_ptr_); break;
    default: throw std::runtime_error("Field type is not correct!");
  }
}

double BranchChannel::operator[](const Field& v) const { return Value(v); }

void BranchChannel::CopyContentRaw(const BranchChannel& other) {
  ANALYSISTREE_UTILS_VISIT(copy_content_struct(), data_ptr_, other.data_ptr_);
}

std::size_t BranchChannel::GetId() const { return ANALYSISTREE_UTILS_VISIT(get_id_struct(), data_ptr_); }
