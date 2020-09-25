//
// Created by eugene on 24/09/2020.
//

#include "BranchView.hpp"

AnalysisTree::IBranchViewPtr AnalysisTree::IBranchView::Select(const std::string& field_name) const {
  using Details::SelectFieldsAction;
  return BranchViewActionResult<SelectFieldsAction>(
      SelectFieldsAction(std::vector<std::string>({field_name})), Clone()).Clone();
}
