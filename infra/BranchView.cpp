//
// Created by eugene on 24/09/2020.
//

#include "BranchView.hpp"

#include "BranchViewAction.hpp"

using namespace AnalysisTree;

AnalysisTree::IBranchViewPtr AnalysisTree::IBranchView::Select(const std::string& field_name) const {
  return Select(std::vector<std::string>({field_name}));
}

IBranchViewPtr IBranchView::Select(const std::vector<std::string>& field_names) const {
  BranchViewAction::SelectFieldsAction action(field_names);
  return Apply(action);
}

IBranchViewPtr IBranchView::operator[](const std::string& field_name) const { return Select(field_name); }

ResultsMCols<double> IBranchView::GetDataMatrix() {
  ResultsMCols<double> result;
  for (auto& column_name : GetFields()) {
    auto emplace_result = result.emplace(column_name, ResultsColumn<double>(GetNumberOfChannels()));
    ResultsColumn<double>& column_vector = emplace_result.first->second;
    IFieldPtr field_ptr = GetFieldPtr(column_name);
    for (size_t i_channel = 0; i_channel < GetNumberOfChannels(); ++i_channel) {
      column_vector[i_channel] = field_ptr->GetValue(i_channel);
    }
  }
  return result;
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
