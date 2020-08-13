#ifndef ANALYSISTREE_PLAINTREEFILLER_H_
#define ANALYSISTREE_PLAINTREEFILLER_H_

#include <utility>

#include "Detector.hpp"
#include "FillTask.hpp"
#include "Variable.hpp"

namespace AnalysisTree {

class PlainTreeFiller : public FillTask {

 public:
  PlainTreeFiller() = default;
  explicit PlainTreeFiller(const std::vector<Variable>& vars) {
    for (const auto& var : vars) {
      vars_.emplace_back(std::make_pair(0.f, var));
    }
  };
  ~PlainTreeFiller() override = default;

  void Init(std::map<std::string, void*>& branches) override;
  void Exec() override;
  void Finish() override {
    out_tree_->Write();
  }

  void SetVariables(const std::vector<Variable>& vars) {
    vars_.reserve(vars.size());
    for (const auto& var : vars) {
      vars_.emplace_back(std::make_pair(0.f, var));
    }
  }

 protected:
  Particles* in_branch_{nullptr};
  std::vector<std::pair<float, Variable>> vars_{};
  TTree* out_tree_{nullptr};
  std::string tree_name_{"PlainTree"};
};

}// namespace AnalysisTree

#endif//ANALYSISTREE_PLAINTREEFILLER_H_
