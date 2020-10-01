//
// Created by eugene on 24/09/2020.
//

#include "BranchView.hpp"
#include "TextTable.h"

#include <sstream>


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
  for (auto &field_name : GetFields()) {
    field_ptrs.emplace_back(GetFieldPtr(field_name));
  }

  TextTable t('-', '|', '+');
  t.add("#ch");
  for (auto &field : GetFields()) {
    t.add(field);
  }
  t.endOfRow();

  for (size_t i_channel = 0; i_channel < GetNumberOfChannels(); ++i_channel) {
    t.add(std::to_string(i_channel));
    for (auto &field : field_ptrs) {
      t.add(std::to_string(field->GetValue(i_channel)));
    }
    t.endOfRow();
  }
  os << t << std::endl;
}
BranchViewPtr IBranchView::RenameFields(const std::map<std::string, std::string>& old_to_new_map) const {
  return Apply(BranchViewAction::RenameFieldsAction(old_to_new_map));
}
BranchViewPtr IBranchView::RenameFields(std::string old_name, std::string new_name) const {
  std::map<std::string, std::string> tmp_map;
  tmp_map.emplace(old_name, new_name);
  return RenameFields(tmp_map);
}
bool IBranchView::HasField(const std::string& name) const {
  auto fields = GetFields();
  return std::find(fields.begin(), fields.end(), name) != fields.end();
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
  for (auto &arg : missing_args) {
    stream << "'" << arg << "'" << " ";
  }
  stream << "missing";
  throw std::out_of_range(stream.str());
}
