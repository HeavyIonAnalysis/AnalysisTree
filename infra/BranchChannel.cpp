/* Copyright (C) 2019-2021 GSI, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Eugeny Kashirin, Ilya Selyuzhenkov */
#include "BranchChannel.hpp"

#include "Branch.hpp"
#include "Field.hpp"
#include "EventHeader.hpp"

using namespace AnalysisTree;

void BranchChannel::UpdateChannel(size_t new_channel) {
  i_channel_ = new_channel;
  UpdatePointer();
}

void BranchChannel::UpdatePointer() {
  if (i_channel_ < branch_->size()) {
    data_ptr_ = std::visit([this](auto&& v){ return ChannelPointer( &(v->Channel(i_channel_)) ); }, branch_->GetData());
  } else {
    throw std::out_of_range("");
  }
}
void BranchChannel::CopyContents(const BranchChannel& other) {
  branch_->CheckMutable();

  auto mapping_it = branch_->copy_fields_mapping.find(other.branch_);
  if (mapping_it == branch_->copy_fields_mapping.end()) {
    branch_->CreateMapping(other.branch_);
    mapping_it = branch_->copy_fields_mapping.find(other.branch_);
  }

  /* Eval mapping */
  const auto& field_pairs = mapping_it->second.field_pairs;

  for (auto& field_pair /* src : dst */ : field_pairs) {
    this->SetValue(field_pair.second, other.Value(field_pair.first));
  }
}

BranchChannel::BranchChannel(const Branch* branch, std::size_t i_channel) : branch_(branch), i_channel_(i_channel) {
  UpdatePointer();
}

BranchChannel Branch::NewChannel() {
  CheckMutable(true);
  ApplyT([this](auto entity_ptr) {
    if constexpr (is_event_header_v<decltype(entity_ptr)>) {
      throw std::runtime_error("Not applicable for EventHeader");
    } else {
      auto channel = entity_ptr->AddChannel();
      channel->Init(this->config_);
      Freeze();
    }
  });
  return operator[](size() - 1);
}

void BranchChannel::Print(std::ostream& os) const {
  os << "Branch " << branch_->GetBranchName() << " channel #" << i_channel_ << std::endl;
}

double BranchChannel::Value(const Field& v) const {
//  assert(v.GetBranchId() == branch_->GetId());
  assert(v.IsInitialized());

  using AnalysisTree::Types;
  switch (v.GetFieldType()) {
    case Types::kFloat :   return ANALYSISTREE_UTILS_VISIT([v](auto&& ch){ return ch->template GetField<float>(v.GetFieldId()); }, data_ptr_);
    case Types::kInteger : return ANALYSISTREE_UTILS_VISIT([v](auto&& ch){ return ch->template GetField<int>(v.GetFieldId()); }, data_ptr_);
    case Types::kBool :    return ANALYSISTREE_UTILS_VISIT([v](auto&& ch){ return ch->template GetField<bool>(v.GetFieldId()); }, data_ptr_);
    default: throw std::runtime_error("Field type is not correct!");
  }
}

void BranchChannel::SetValue(const Field& v, double value) {
  using AnalysisTree::Types;
  switch (v.GetFieldType()) {
    case Types::kFloat :   ANALYSISTREE_UTILS_VISIT([v, value](auto&& ch){ ch->template SetField<float>(v.GetFieldId(), value); }, data_ptr_);
    case Types::kInteger : ANALYSISTREE_UTILS_VISIT([v, value](auto&& ch){ ch->template SetField<int>(v.GetFieldId(), value); }, data_ptr_);
    case Types::kBool :    ANALYSISTREE_UTILS_VISIT([v, value](auto&& ch){ ch->template SetField<bool>(v.GetFieldId(), value); }, data_ptr_);
    default: throw std::runtime_error("Field type is not correct!");
  }
}

double BranchChannel::operator[](const Field& v) const { return Value(v); }
