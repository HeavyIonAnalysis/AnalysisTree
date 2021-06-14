/* Copyright (C) 2019-2021 GSI, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Eugeny Kashirin, Ilya Selyuzhenkov */
#include <TTree.h>

#include "Branch.hpp"
#include "Variable.hpp"
#include <Constants.hpp>

#include <string>

#include <cassert>
#include <iostream>
#include <map>
#include <vector>

using namespace AnalysisTree;

namespace Impl {

inline void hash_combine(std::size_t& seed) {}

template<typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  hash_combine(seed, rest...);
}

std::size_t BranchConfigHasher(const AnalysisTree::BranchConfig& config) {
  using Type = AnalysisTree::Types;

  std::size_t hash = 0;
  hash_combine(hash, config.GetType());

  auto hash_fields = [&hash](const std::map<std::string, ConfigElement>& fields_map, Type field_type) {
    for (auto& field : fields_map) {
      hash_combine(hash, field.first, field.second.id_, field_type);
    }
  };

  hash_fields(config.GetMap<float>(), Type::kFloat);
  hash_fields(config.GetMap<int>(), Type::kInteger);
  hash_fields(config.GetMap<bool>(), Type::kBool);
  return hash;
}

}// namespace Impl

Field Branch::GetFieldVar(const std::string& field_name) {
  AnalysisTree::Field v;
  v.parent_branch = this;
  v.field_id_ = v.parent_branch->config_.GetFieldId(field_name);
  //  v.name_ = this->config.GetName() + "/" + field_name; //NOTE is it needed?
  v.field_ = field_name;
  v.field_type_ = config_.GetFieldType(field_name);
  v.is_init_ = true;

  if (v.field_id_ == AnalysisTree::UndefValueShort)
    throw std::runtime_error("Field of name '" + v.field_ + "' not found");
  return v;
}

Branch::~Branch() {
  ApplyT([this](auto entry_ptr) {
    delete entry_ptr;
  });
}

void Branch::ConnectOutputTree(TTree* tree) {
  is_connected_to_output = ApplyT([this, tree](auto entity) -> bool {
    if (!tree)
      return false;
    auto new_tree_branch_ptr = tree->Branch(config_.GetName().c_str(),
                                            std::add_pointer_t<decltype(entity)>(&this->data_));
    return bool(new_tree_branch_ptr);
  });
}

void Branch::InitDataPtr() {
  ApplyT([this](auto entity) {
    if (entity)
      throw std::runtime_error("Data ptr is already initialized");
    auto entity_id = Hash();
    this->data_ = new typename std::remove_pointer<decltype(entity)>::type(entity_id);
  });
}

size_t AnalysisTree::Branch::size() const {
  return std::visit([](auto&& arg) { return arg->GetNumberOfChannels(); }, data_);
}

Field Branch::NewVariable(const std::string& field_name, AnalysisTree::Types type) {
  if (field_name.empty())
    throw std::runtime_error("Field name cannot be empty");
  if (type == AnalysisTree::Types::kNumberOfTypes)
    throw std::runtime_error("Type of the field cannot be kNumberOfTypes");
  if (HasField(field_name)) {
    throw std::runtime_error("Field of name '" + field_name + "' already exists in the config");
  }

  CheckFrozen(false);
  CheckMutable(true);
  using AnalysisTree::Types;

  if (Types::kFloat == type) {
    config_.template AddField<float>(field_name);
  } else if (Types::kInteger == type) {
    config_.template AddField<int>(field_name);
  } else if (Types::kBool == type) {
    config_.template AddField<bool>(field_name);
  } else {
    /* should never happen */
    assert(false);
  }

  UpdateConfigHash();

  /* Init EventHeader */
  if (AnalysisTree::DetType::kEventHeader == config_.GetType()) {
    ANALYSISTREE_UTILS_GET<AnalysisTree::EventHeader*>(data_)->Init(config_);
  }

  AnalysisTree::Field v;
  //  v.name = config.GetName() + "/" + field_name;
  v.field_ = field_name;
  v.parent_branch = this;
  v.field_id_ = config_.GetFieldId(field_name);
  v.field_type_ = config_.GetFieldType(field_name);
  v.is_init_ = true;
  return v;
}

void Branch::CloneVariables(const AnalysisTree::BranchConfig& other) {
  auto import_fields_from_map = [this](const std::map<std::string, ConfigElement>& map, AnalysisTree::Types type) {
    for (auto& element : map) {
      auto field_name = element.first;
      if (HasField(field_name)) {
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

void Branch::ClearChannels() {
  CheckMutable();
  ApplyT([](auto entity_ptr) -> void {
    if constexpr (is_event_header_v<decltype(entity_ptr)>) {
      throw std::runtime_error("Not applicable for EventHeader");
    } else {
      entity_ptr->ClearChannels();
    }
  });
}
bool Branch::HasField(const std::string& field_name) const {
  auto has_field = [&field_name](const std::map<std::string, ConfigElement>& map) {
    return map.find(field_name) != map.end();
  };
  return has_field(config_.GetMap<float>()) || has_field(config_.GetMap<int>()) || has_field(config_.GetMap<bool>());
}
std::vector<std::string> Branch::GetFieldNames() const {
  std::vector<std::string> result;
  auto fill_vector_from_map = [&result](const std::map<std::string, ConfigElement>& fields_map) -> void {
    for (auto& element : fields_map) {
      result.push_back(element.first);
    }
  };
  fill_vector_from_map(config_.GetMap<float>());
  fill_vector_from_map(config_.GetMap<int>());
  fill_vector_from_map(config_.GetMap<bool>());
  return result;
}

//void Branch::CopyContents(Branch* other) {
//  if (this == other) {
//    throw std::runtime_error("Copying contents from the same branch makes no sense");
//  }
//  CheckMutable();
//
//  if (other->config.GetType() != config.GetType()) {
//    throw std::runtime_error("Branch types must be the same");
//  }
//  if (config.GetType() != AnalysisTree::DetType::kEventHeader) {
//    throw std::runtime_error("Only EventHeader is available for Branch::CopyContents");
//  }
//
//  auto mapping_it = copy_fields_mapping.find(other);
//  if (mapping_it == copy_fields_mapping.end()) {
//    CreateMapping(other);
//    mapping_it = copy_fields_mapping.find(other);
//  }
//
//  /* evaluate mapping */
//  auto src_branch = mapping_it->first;
//  const auto& mapping = mapping_it->second;
//
//  for (auto& field_pair /* src : dst */ : mapping.field_pairs) {
//    this->Value(field_pair.second) = src_branch->Value(field_pair.first);
//  }
//}

void Branch::CopyContentsRaw(Branch* other) {
//  if (this == other) {
//    throw std::runtime_error("Copying contents from the same branch makes no sense");
//  }
//  CheckMutable();
//  CheckFrozen();
//
//  if (config_hash_ != other->config_hash_) {
//    throw std::runtime_error("Branch configurations are not consistent.");
//  }
//  auto src_data_ptr = other->data;
//  ApplyT([src_data_ptr](auto dst_data_ptr) {
//    auto typed_src_data_ptr = reinterpret_cast<decltype(dst_data_ptr)>(src_data_ptr);
//    *dst_data_ptr = *typed_src_data_ptr;
//  });
}

void Branch::CreateMapping(Branch* other) {
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

  std::cout << "New cached mapping " << other->config_.GetName() << " --> " << config_.GetName() << std::endl;
  FieldsMapping fields_mapping;
  for (auto& field_name : other->GetFieldNames()) {
    if (!HasField(field_name)) { continue; }
    fields_mapping.field_pairs.emplace_back(std::make_pair(other->GetFieldVar(field_name), GetFieldVar(field_name)));
    std::cout << "\t" << field_name
              << "\t(" << types_map.at(other->GetFieldVar(field_name).GetFieldType()) << " ---> "
              << types_map.at(GetFieldVar(field_name).GetFieldType()) << ")" << std::endl;
  }
  copy_fields_mapping.emplace(other, std::move(fields_mapping));
}
void Branch::UseFields(std::vector<std::pair<std::string, std::reference_wrapper<Field>>>&& vars,
                       bool ignore_missing) {
  for (auto& element : vars) {
    auto& field_name = element.first;
    if (!HasField(field_name) && ignore_missing) {
      element.second.get() = Field();
      continue;
    }
    element.second.get() = GetFieldVar(field_name);
  }
}
void Branch::UpdateConfigHash() {
  config_hash_ = Impl::BranchConfigHasher(config_);
}
