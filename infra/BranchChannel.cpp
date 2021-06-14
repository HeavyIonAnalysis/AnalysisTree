/* Copyright (C) 2019-2021 GSI, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Eugeny Kashirin, Ilya Selyuzhenkov */
#include "BranchChannel.hpp"
#include "Branch.hpp"

using namespace AnalysisTree;

void BranchChannel::UpdateChannel(size_t new_channel) {
  i_channel = new_channel;
  UpdatePointer();
}
void BranchChannel::UpdatePointer() {
  if (i_channel < branch->size()) {
    data_ptr = std::visit([this](auto&& v){ return ChannelPointer( &(v->Channel(i_channel)) ); }, branch->GetData());
  } else {
    throw std::out_of_range("");
  }
}
void BranchChannel::CopyContents(const BranchChannel& other) {
//  branch->CheckMutable();
//
//  auto mapping_it = branch->copy_fields_mapping.find(other.branch);
//  if (mapping_it == branch->copy_fields_mapping.end()) {
//    branch->CreateMapping(other.branch);
//    mapping_it = branch->copy_fields_mapping.find(other.branch);
//  }
//
//  /* Eval mapping */
//  const auto& field_pairs = mapping_it->second.field_pairs;
//
//  for (auto& field_pair /* src : dst */ : field_pairs) {
//    this->Value(field_pair.second) = other.Value(field_pair.first);
//  }
}
void BranchChannel::CopyContents(Branch& other) {
//  branch->CheckMutable();
//
//  if (other.GetBranchType() != AnalysisTree::DetType::kEventHeader) {
//    throw std::runtime_error("This operation is allowed only between iterable and non-iterable types");
//  }
//
//  auto mapping_it = branch->copy_fields_mapping.find(&other);
//  if (mapping_it == branch->copy_fields_mapping.end()) {
//    branch->CreateMapping(&other);
//    mapping_it = branch->copy_fields_mapping.find(&other);
//  }
//
//  /* Eval mapping */
//  const auto& field_pairs = mapping_it->second.field_pairs;
//
//  for (auto& field_pair /* src : dst */ : field_pairs) {
//    this->Value(field_pair.second) = other.Value(field_pair.first);
//  }
}

BranchChannel::BranchChannel(Branch* branch, std::size_t i_channel) : branch(branch), i_channel(i_channel) {
  UpdatePointer();
}

//BranchChannel Branch::operator[](size_t i_channel) { return BranchChannel(this, i_channel); }
BranchChannel Branch::NewChannel() {
//  CheckMutable(true);
//  ApplyT([this](auto entity_ptr) {
//    if constexpr (is_event_header_v<decltype(entity_ptr)>) {
//      throw std::runtime_error("Not applicable for EventHeader");
//    } else {
//      auto channel = entity_ptr->AddChannel();
//      channel->Init(this->config_);
//      Freeze();
//    }
//  });
//  return operator[](size() - 1);
}

void BranchChannel::Print(std::ostream& os) const {
  os << "Branch " << branch->GetBranchName() << " channel #" << i_channel << std::endl;
}