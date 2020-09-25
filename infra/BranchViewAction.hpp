//
// Created by eugene on 25/09/2020.
//

#ifndef ANALYSISTREE_INFRA_BRANCHREADERACTION_HPP
#define ANALYSISTREE_INFRA_BRANCHREADERACTION_HPP

#include <algorithm>

#include <AnalysisTree/BranchView.hpp>
#include <utility>

namespace AnalysisTree {

namespace BranchViewAction {

class SelectFieldsAction {

  class SelectFieldsActionResultImpl : public IBranchView {
   public:
    std::vector<std::string> GetFields() const override {
      return selected_fields_;
    }
    size_t GetNumberOfChannels() const override {
      return origin_->GetNumberOfChannels();
    }
    void GetEntry(Long64_t entry) const override {
      return origin_->GetEntry(entry);
    }
    IBranchViewPtr Clone() const override {
      auto newView = std::make_shared<SelectFieldsActionResultImpl>();
      newView->origin_ = origin_->Clone();
      newView->selected_fields_ = selected_fields_;
      return newView;
    }
    ResultsMCols<double> GetDataMatrix() override {
      ResultsMCols<double> result;
      auto origin_matrix = origin_->GetDataMatrix();
      for (auto &field : selected_fields_) {
        result.emplace(field, origin_matrix.at(field));
      }
      return result;
    }

    IBranchViewPtr origin_;
    std::vector<std::string> selected_fields_;

  };

 public:
  SelectFieldsAction(std::vector<std::string> selectedFields) : selected_fields_(std::move(selectedFields)) {}
  IBranchViewPtr ApplyAction(const IBranchViewPtr& origin) {
    auto origin_fields = origin->GetFields();
    std::vector<std::string> intersection;

    std::set_intersection(origin_fields.begin(), origin_fields.end(),
                          selected_fields_.begin(), selected_fields_.end(),
                          std::back_inserter(intersection));

    auto action_result = std::make_shared<SelectFieldsActionResultImpl>();
    action_result->selected_fields_ = intersection;
    action_result->origin_ = origin;
    return action_result;
  }

 private:
  std::vector<std::string> selected_fields_;

};



}


}

#endif//ANALYSISTREE_INFRA_BRANCHREADERACTION_HPP
