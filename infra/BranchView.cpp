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
