#ifndef ANALYSISTREE_EXAMPLES_USERTASKWRITE_HPP_
#define ANALYSISTREE_EXAMPLES_USERTASKWRITE_HPP_

#include <Detector.hpp>
#include <AnalysisTree/Branch.hpp>
#include <AnalysisTree/Task.hpp>

class UserTaskWrite : public AnalysisTree::Task {

 public:
  UserTaskWrite() = default;
  ~UserTaskWrite() override;

  void Init() override;
  void Exec() override;

  void Finish() override {
  }

 private:
  AnalysisTree::Branch particles_;
  AnalysisTree::Branch new_particles_;

  AnalysisTree::Particles* new_particles_ptr_{nullptr};
};
#endif//ANALYSISTREE_EXAMPLES_USERTASKWRITE_HPP_
