#ifndef ANALYSISTREE_EXAMPLES_USERTASKREAD_HPP_
#define ANALYSISTREE_EXAMPLES_USERTASKREAD_HPP_

#include <AnalysisTree/Task.hpp>

class UserTaskRead : public AnalysisTree::Task {

 public:
  UserTaskRead() = default;
  ~UserTaskRead() override;

  void Init() override;
  void Exec() override;

  void Finish() override {}

 private:
  AnalysisTree::Branch particles_;
  AnalysisTree::Branch tracks_;
};

#endif//ANALYSISTREE_EXAMPLES_USERTASKREAD_HPP_
