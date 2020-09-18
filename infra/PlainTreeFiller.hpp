#ifndef ANALYSISTREE_PLAINTREEFILLER_H_
#define ANALYSISTREE_PLAINTREEFILLER_H_

#include <utility>

#include "Detector.hpp"
#include "VarManager.hpp"
#include "Variable.hpp"

namespace AnalysisTree {

class PlainTreeFiller : public VarManager {
 public:
  PlainTreeFiller() = default;

  void AddBranch(const std::string& branch_name);

  void Init(std::map<std::string, void*>& branches) override;
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
