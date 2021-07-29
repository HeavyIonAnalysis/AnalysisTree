/* Copyright (C) 2019-2021 GSI, MEPhI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Eugeny Kashirin, Viktor Klochkov, Ilya Selyuzhenkov */
#include <TTree.h>

#include "Branch.hpp"
#include "BranchHashHelper.hpp"

#include <Constants.hpp>

#include <string>

#include <cassert>
#include <iostream>
#include <map>
#include <vector>

using namespace AnalysisTree;

void Branch::InitDataPtr() {
  switch (config_.GetType()) {
    case DetType::kParticle: {
      data_ = new Particles(config_.GetId());
      break;
    }
    case DetType::kTrack: {
      data_ = new TrackDetector(config_.GetId());
      break;
    }
    case DetType::kHit: {
      data_ = new HitDetector(config_.GetId());
      break;
    }
    case DetType::kModule: {
      data_ = new ModuleDetector(config_.GetId());
      break;
    }
    case DetType::kEventHeader: {
      data_ = new EventHeader(config_.GetId());
      break;
    }
    default: throw std::runtime_error("Branch type is not known!");
  }
}

Field Branch::GetFieldVar(const std::string& field_name) {
  AnalysisTree::Field v;
  v.parent_branch_ = this;
  v.field_id_ = v.parent_branch_->config_.GetFieldId(field_name);
  //  v.name_ = this->config.GetName() + "/" + field_name; //NOTE is it needed?
  v.field_ = field_name;
  v.field_type_ = config_.GetFieldType(field_name);
  v.is_init_ = true;

  if (v.field_id_ == AnalysisTree::UndefValueShort)
    throw std::runtime_error("Field of name '" + v.field_ + "' not found");
  return v;
}

size_t AnalysisTree::Branch::size() const {
  return ANALYSISTREE_UTILS_VISIT(get_n_channels_struct(), data_);
}

Field Branch::NewVariable(const std::string& field_name, AnalysisTree::Types type) {
  if (field_name.empty()) {
    throw std::runtime_error("Field name cannot be empty");
  }
  if (type == AnalysisTree::Types::kNumberOfTypes) {
    throw std::runtime_error("Type of the field cannot be kNumberOfTypes");
  }
  if (config_.HasField(field_name)) {
    throw std::runtime_error("Field of name '" + field_name + "' already exists in the config");
  }

  CheckFrozen(false);
  CheckMutable(true);
  using AnalysisTree::Types;

  switch (type) {
    case Types::kFloat: {
      config_.template AddField<float>(field_name);
      break;
    }
    case Types::kInteger: {
      config_.template AddField<int>(field_name);
      break;
    }
    case Types::kBool: {
      config_.template AddField<bool>(field_name);
      break;
    }
    default: assert(false);
  }

  UpdateConfigHash();

  /* Init EventHeader */
  if (config_.GetType() == AnalysisTree::DetType::kEventHeader) {
    ANALYSISTREE_UTILS_GET<AnalysisTree::EventHeader*>(data_)->Init(config_);
  }

  AnalysisTree::Field v;
  v.field_ = field_name;
  v.parent_branch_ = this;
  v.field_id_ = config_.GetFieldId(field_name);
  v.field_type_ = config_.GetFieldType(field_name);
  v.is_init_ = true;
  return v;
}

void Branch::ClearChannels() {
  CheckMutable();
  ANALYSISTREE_UTILS_VISIT(clear_channels_struct(), data_);
}

void Branch::ConnectOutputTree(TTree* /*tree*/) {
  //  is_connected_to_output = ApplyT([this, tree](auto entity) -> bool {
  //    if (!tree)
  //      return false;
  //    auto new_tree_branch_ptr = tree->Branch(config_.GetName().c_str(),
  //                                            std::add_pointer_t<decltype(entity)>(&this->data_));
  //    return bool(new_tree_branch_ptr);
  //  });
}

void Branch::CloneVariables(const AnalysisTree::BranchConfig& other) {
  auto import_fields_from_map = [this](const std::map<std::string, ConfigElement>& map, AnalysisTree::Types type) {
    for (auto& element : map) {
      auto field_name = element.first;
      if (config_.HasField(field_name)) {
        std::cout << "Field '" << field_name << "' already exists" << std::endl;
        continue;
      }
      this->NewVariable(field_name, type);
    }// map elements
  };

  import_fields_from_map(other.GetMap<float>(), AnalysisTree::Types::kFloat);
  import_fields_from_map(other.GetMap<int>(), AnalysisTree::Types::kInteger);
  import_fields_from_map(other.GetMap<bool>(), AnalysisTree::Types::kBool);
}

void Branch::CopyContents(Branch* other) {
  if (this == other) {
    throw std::runtime_error("Copying contents from the same branch makes no sense");
  }
  CheckMutable();

  if (other->config_.GetType() != config_.GetType()) {
    throw std::runtime_error("Branch types must be the same");
  }
  if (config_.GetType() != AnalysisTree::DetType::kEventHeader) {
    throw std::runtime_error("Only EventHeader is available for Branch::CopyContents");
  }

  auto mapping_it = copy_fields_mapping.find(other);
  if (mapping_it == copy_fields_mapping.end()) {
    CreateMapping(other);
    mapping_it = copy_fields_mapping.find(other);
  }

  /* evaluate mapping */
  auto src_branch = mapping_it->first;
  const auto& mapping = mapping_it->second;

  //  for (auto& field_pair /* src : dst */ : mapping.field_pairs) {
  //    this->SetValue(field_pair.second, src_branch->Value(field_pair.first));
  //  }
}

void Branch::CopyContentsRaw(Branch* other) {
  if (this == other) {
    throw std::runtime_error("Copying contents from the same branch makes no sense");
  }
  CheckMutable();
  //  CheckFrozen();

  if (config_hash_ != other->config_hash_) {
    throw std::runtime_error("Branch configurations are not consistent.");
  }

  if (this->GetBranchType() != other->GetBranchType()) {
    throw std::runtime_error("Branches have different type.");
  }

  switch (this->GetBranchType()) {
    case DetType::kParticle: {
      *ANALYSISTREE_UTILS_GET<Particles*>(data_) = *ANALYSISTREE_UTILS_GET<Particles*>(other->data_);
      break;
    }
    case DetType::kTrack: {
      *ANALYSISTREE_UTILS_GET<TrackDetector*>(data_) = *ANALYSISTREE_UTILS_GET<TrackDetector*>(other->data_);
      break;
    }
    case DetType::kHit: {
      *ANALYSISTREE_UTILS_GET<HitDetector*>(data_) = *ANALYSISTREE_UTILS_GET<HitDetector*>(other->data_);
      break;
    }
    case DetType::kModule: {
      *ANALYSISTREE_UTILS_GET<ModuleDetector*>(data_) = *ANALYSISTREE_UTILS_GET<ModuleDetector*>(other->data_);
      break;
    }
    case DetType::kEventHeader: {
      *ANALYSISTREE_UTILS_GET<EventHeader*>(data_) = *ANALYSISTREE_UTILS_GET<EventHeader*>(other->data_);
      break;
    }
  }
}

void Branch::CreateMapping(const Branch* other) const {
  if (copy_fields_mapping.find(other) != copy_fields_mapping.end()) {
    // TODO Warning
    return;
  }

  CheckFrozen();
  other->CheckFrozen();

  const std::map<AnalysisTree::Types, std::string> types_map = {
      {AnalysisTree::Types::kFloat, "float"},
      {AnalysisTree::Types::kInteger, "integer"},
      {AnalysisTree::Types::kBool, "bool"}};

  //  std::cout << "New cached mapping " << other->config_.GetName() << " --> " << config_.GetName() << std::endl;
  //  FieldsMapping fields_mapping;
  //  for (auto& field_name : other->GetFieldNames()) {
  //    if (!config_.HasField(field_name)) { continue; }
  //    fields_mapping.field_pairs.emplace_back(std::make_pair(other->GetFieldVar(field_name), GetFieldVar(field_name)));
  //    std::cout << "\t" << field_name
  //              << "\t(" << types_map.at(other->GetFieldVar(field_name).GetFieldType()) << " ---> "
  //              << types_map.at(GetFieldVar(field_name).GetFieldType()) << ")" << std::endl;
  //  }
  //  copy_fields_mapping.emplace(other, std::move(fields_mapping));
}

//void Branch::UseFields(std::vector<std::pair<std::string, std::reference_wrapper<Field>>>&& vars,
//                       bool ignore_missing) {
//  for (auto& element : vars) {
//    auto& field_name = element.first;
//    if (!config_.HasField(field_name) && ignore_missing) {
//      element.second.get() = Field();
//      continue;
//    }
//    element.second.get() = GetFieldVar(field_name);
//  }
//}

void Branch::UpdateConfigHash() {
  config_hash_ = Impl::BranchConfigHasher(config_);
}

Branch::~Branch() {
  //  ApplyT([this](auto entry_ptr) {  // TODO
  //    delete entry_ptr;
  //  });
}