/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include <iostream>

#include <AnalysisTree/ToyMC.hpp>
#include <AnalysisTree/Branch.hpp>

using namespace AnalysisTree;

class WriteTask : public Task {

 public:
  WriteTask() = default;

  void Init() override {
    auto man = TaskManager::GetInstance();
    auto chain = man->GetChain();

    in_branches_.emplace("SimParticles");

    auto br_conf = chain->GetConfiguration()->GetBranchConfig("SimParticles");
    auto new_conf = br_conf.Clone("NewParticles", DetType::kParticle);
    auto new_conf1 = br_conf.Clone("NewParticles1", DetType::kParticle);

    man->AddBranch(ptr_, new_conf);

    new_particles_ = new Branch(new_conf1);
    new_particles_->SetMutable();
    man->AddBranch(new_particles_);

    auto ptr = chain->GetPointerToBranch("SimParticles");
    particles_ = new Branch(br_conf, ptr);
  }
  void Exec() override {
//    ptr = std::get<Particles*>(particles_.GetData());
    std::cout << particles_->size() << std::endl;
    *ptr_ = *(particles_->GetDataRaw<Particles*>());

    //    *(new_particles_->GetDataRaw<Particles*>()) = *ptr_;

    new_particles_->CopyContentsRaw(particles_);

  }
  void Finish() override {
  }

 private:
  Branch* particles_{nullptr};
  Branch* new_particles_{nullptr};

  Particles* ptr_{nullptr};

};

int main(int argc, char* argv[]) {

  const int n_events = 10;
  const std::string filelist = "fl_toy_mc.txt";

  RunToyMC(n_events, filelist);

  auto* man = TaskManager::GetInstance();
  man->SetOutputMode(eBranchWriteMode::kCopyTree);

  auto* task = new WriteTask();
  man->AddTask(task);

  man->Init({filelist}, {"tTree"});
  man->Run(-1);
  man->Finish();
}
