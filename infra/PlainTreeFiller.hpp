#ifndef ANALYSISTREE_PLAINTREEFILLER_H_
#define ANALYSISTREE_PLAINTREEFILLER_H_

#include <utility>

#include "Detector.hpp"
#include "AnalysisTask.hpp"
#include "Variable.hpp"

namespace AnalysisTree {

class PlainTreeFiller : public AnalysisTask {
 public:
  PlainTreeFiller() = default;

  void AddBranch(const std::string& branch_name);

  void Init() override;
  void Exec() override;
  void Finish() override;

 protected:
  TTree* plain_tree_{nullptr};
  std::string tree_name_{"PlainTree"};
  std::string branch_name_;

  std::vector<float> vars_{};
};

}// namespace AnalysisTree

#endif//ANALYSISTREE_PLAINTREEFILLER_H_
