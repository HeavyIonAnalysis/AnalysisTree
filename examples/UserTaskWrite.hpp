#ifndef ANALYSISTREE_EXAMPLES_USERTASKWRITE_HPP_
#define ANALYSISTREE_EXAMPLES_USERTASKWRITE_HPP_

#include <AnalysisTree/Branch.hpp>

using namespace AnalysisTree;

class WriteTask : public Task {

 public:
  WriteTask() = default;

  void Init() override {
    auto man = TaskManager::GetInstance();
    auto chain = man->GetChain();

    auto ptr = chain->GetPointerToBranch("SimParticles");
    auto br_conf = chain->GetConfiguration()->GetBranchConfig("SimParticles");

    particles_ = new Branch(br_conf, ptr);
    in_branches_.emplace("SimParticles");

    auto new_conf = br_conf.Clone("NewParticles", DetType::kParticle);
    man->AddBranch("NewParticles", ptr_, new_conf, eBranchWriteMode::kCopyTree);
  }
  void Exec() override {
//    ptr = std::get<Particles*>(particles_.GetData());
//    std::cout << particles_->size() << std::endl;
  }
  void Finish() override {
  }

 private:
  Branch* particles_;
  Branch* new_particles_;
  Particles* ptr_{nullptr};
};

#endif//ANALYSISTREE_EXAMPLES_USERTASKWRITE_HPP_
