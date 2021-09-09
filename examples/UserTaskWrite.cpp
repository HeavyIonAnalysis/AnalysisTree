#include "UserTaskWrite.hpp"

#include "AnalysisTree/TaskManager.hpp"

using namespace AnalysisTree;

void UserTaskWrite::Init() {
  auto man = TaskManager::GetInstance();
  auto chain = man->GetChain();

  in_branches_.emplace("SimParticles");
  particles_ = chain->GetBranch("SimParticles");

  auto br_conf = chain->GetConfiguration()->GetBranchConfig("SimParticles");
  auto new_conf = br_conf.Clone("NewParticles", DetType::kParticle);
  auto new_conf1 = br_conf.Clone("NewParticles1", DetType::kParticle);

  man->AddBranch(new_particles_ptr_, new_conf);

  new_particles_ = Branch(new_conf1);
  new_particles_.SetMutable();
  man->AddBranch(&new_particles_);
}

void UserTaskWrite::Exec() {

//  new_particles_.CopyContentsRaw(&particles_);

  for(int i=0; i<particles_.size(); ++i){
    auto particle = particles_[i];
    auto new_part = new_particles_.NewChannel();
    new_part.CopyContent(particle);
//    new_part.CopyContentRaw(particle);
  }

}

UserTaskWrite::~UserTaskWrite() {
  delete new_particles_ptr_;
}