#include "BranchView.hpp"
#include "TextTable.h"

#include <set>
#include <sstream>
#include <utility>

using namespace AnalysisTree;

AnalysisTree::BranchViewPtr AnalysisTree::IBranchView::Select(const std::string& field_name) const {
  return Select(std::vector<std::string>({field_name}));
}

BranchViewPtr IBranchView::Select(const std::vector<std::string>& field_names) const {
  BranchViewAction::SelectFieldsAction action(field_names);
  return Apply(action);
}

BranchViewPtr IBranchView::operator[](const std::string& field_name) const { return Select(field_name); }

ResultsMCols<double> IBranchView::GetDataMatrix() {
  ResultsMCols<double> result;
  for (auto& column_name : GetFields()) {
    auto emplace_result = result.emplace(column_name, ResultsColumn<double>(GetNumberOfChannels()));
    ResultsColumn<double>& column_vector = emplace_result.first->second;
    FieldPtr field_ptr = GetFieldPtr(column_name);
    for (size_t i_channel = 0; i_channel < GetNumberOfChannels(); ++i_channel) {
      column_vector[i_channel] = field_ptr->GetValue(i_channel);
    }
  }
  return result;
}

void IBranchView::PrintEntry(std::ostream& os) {

  std::vector<FieldPtr> field_ptrs;
  field_ptrs.reserve(GetFields().size());
  for (auto& field_name : GetFields()) {
    field_ptrs.emplace_back(GetFieldPtr(field_name));
  }

  TextTable t('-', '|', '+');
  t.add("#ch");
  for (auto& field : GetFields()) {
    t.add(field);
  }
  t.endOfRow();

  for (size_t i_channel = 0; i_channel < GetNumberOfChannels(); ++i_channel) {
    t.add(std::to_string(i_channel));
    for (auto& field : field_ptrs) {
      t.add(std::to_string(field->GetValue(i_channel)));
    }
    t.endOfRow();
  }
  os << t << std::endl;
}
BranchViewPtr IBranchView::RenameFields(const std::map<std::string, std::string>& old_to_new_map) const {
  return Apply(BranchViewAction::RenameFieldsAction(old_to_new_map));
}
BranchViewPtr IBranchView::RenameFields(const std::string& old_name, const std::string& new_name) const {
  std::map<std::string, std::string> tmp_map;
  tmp_map.emplace(old_name, new_name);
  return RenameFields(tmp_map);
}
bool IBranchView::HasField(const std::string& name) const {
  auto fields = GetFields();
  return std::find(fields.begin(), fields.end(), name) != fields.end();
}
BranchViewPtr IBranchView::AddPrefix(const std::string& prefix) const {
  std::map<std::string, std::string> rename_map;
  for (auto& field : GetFields()) {
    rename_map.emplace(field, prefix + field);
  }
  return RenameFields(rename_map);
}

BranchViewPtr IBranchView::Merge(const IBranchView& right, std::string left_prefix, std::string right_prefix) const {
  BranchViewAction::CombiningMergeAction action(right.Clone(), std::move(right_prefix), std::move(left_prefix));
  return Apply(action);
}
BranchViewPtr IBranchView::Merge(const BranchViewPtr& right, const std::string& left_prefix, const std::string& right_prefix) const {
  return Merge(*right, left_prefix, right_prefix);
}

std::vector<std::string> BranchViewAction::Details::GetMissingArgs(const std::vector<std::string>& args, const std::vector<std::string>& view_fields) {
  std::vector<std::string> result;

  for (auto& arg : args) {
    if (std::find(view_fields.begin(), view_fields.end(), arg) == view_fields.end()) {
      result.emplace_back(arg);
    }
  }
  return result;
}
void BranchViewAction::Details::ThrowMissingArgs(const std::vector<std::string>& missing_args) {
  if (missing_args.empty()) {
    return;
  }
  std::stringstream stream;
  stream << "Args ";
  for (auto& arg : missing_args) {
    stream << "'" << arg << "'"
           << " ";
  }
  stream << "missing";
  throw std::out_of_range(stream.str());
}

BranchViewPtr BranchViewAction::RenameFieldsAction::ApplyAction(const BranchViewPtr& origin) {
  std::map<std::string, std::string> new_to_old_map;
  std::vector<std::string> fields_to_rename;
  fields_to_rename.reserve(old_to_new_map_.size());
  auto origin_fields = origin->GetFields();
  std::vector<std::string> fields_after_rename(origin_fields.begin(), origin_fields.end());

  /* invert map, make sure there is no duplication in new set */
  for (auto& entry : old_to_new_map_) {
    auto old_value = entry.first;
    auto new_value = entry.second;
    fields_to_rename.emplace_back(old_value);
    auto emplace_result = new_to_old_map.emplace(new_value, old_value);
    /* check if no duplications on the right side of old_to_new map */
    if (!emplace_result.second) {
      throw std::runtime_error("New field '" + new_value + "' is duplicated in the map");
    }

    auto field_position = std::distance(origin_fields.begin(), std::find(origin_fields.begin(), origin_fields.end(), old_value));
    fields_after_rename.at(field_position) = new_value;
  }

  {
    std::set<std::string> tmp(fields_after_rename.begin(), fields_after_rename.end());
    if (tmp.size() < fields_after_rename.size()) {
      throw std::runtime_error("Duplicated fields after rename");
    }
  }

  auto missing_args = Details::GetMissingArgs(fields_to_rename, origin_fields);
  Details::ThrowMissingArgs(missing_args);

  return std::make_shared<RenameFieldsActionResultImpl>(fields_after_rename, new_to_old_map, origin);
}

BranchViewPtr BranchViewAction::CombiningMergeAction::ApplyAction(const BranchViewPtr& left) {
  /* same number of events */
  if (left->GetEntries() != right_->GetEntries()) {
    throw std::runtime_error("Two BranchViews MUST have same number of events to be merged!");
  }
  /* overlapping fields */
  auto left_fields = left->GetFields();
  auto right_fields = right_->GetFields();
  std::sort(left_fields.begin(), left_fields.end());
  std::sort(right_fields.begin(), right_fields.end());
  std::vector<std::string> intersection;
  std::set_intersection(left_fields.begin(), left_fields.end(),
                        right_fields.begin(), right_fields.end(),
                        std::back_inserter(intersection));

  auto left_arg = left;
  auto right_arg = right_;
  if (!intersection.empty()) {
    if (left_prefix_ == right_prefix_) {
      throw std::runtime_error("left_prefix and right prefix must be different (not both empty also)");
    }

    if (!left_prefix_.empty()) {
      std::map<std::string, std::string> rename_map;
      for (auto& field : intersection) {
        rename_map.emplace(field, left_prefix_ + field);
      }
      left_arg = left->RenameFields(rename_map);
    }

    if (!right_prefix_.empty()) {
      std::map<std::string, std::string> rename_map;
      for (auto& field : intersection) {
        rename_map.emplace(field, right_prefix_ + field);
      }
      right_arg = right_->RenameFields(rename_map);
    }
  }// !intersection.empty()

  return std::make_shared<CombiningMergeActionResultImpl>(left_arg, right_arg);
}
BranchViewAction::CombiningMergeAction::CombiningMergeActionResultImpl::CombiningMergeActionResultImpl(BranchViewPtr left, BranchViewPtr right) : left_(std::move(left)), right_(std::move(right)) {
  left_index_ = std::make_shared<ChannelIndex>();
  left_index_->side = ChannelIndex::LEFT;
  right_index_ = std::make_shared<ChannelIndex>();
  right_index_->side = ChannelIndex::RIGHT;

  auto left_fields = left_->GetFields();
  auto right_fields = right_->GetFields();
  field_names_.reserve(left_fields.size() + right_fields.size());
  std::copy(left_fields.begin(), left_fields.end(), std::back_inserter(field_names_));
  std::copy(right_fields.begin(), right_fields.end(), std::back_inserter(field_names_));

  for (auto& field_name : left_->GetFields()) {
    fields_.emplace(field_name, std::make_shared<FieldImpl>(left_index_, left_->GetFieldPtr(field_name)));
  }

  for (auto& field_name : right_->GetFields()) {
    fields_.emplace(field_name, std::make_shared<FieldImpl>(right_index_, right_->GetFieldPtr(field_name)));
  }
}
