#include "UserTaskWrite.hpp"

#include "AnalysisTree/TaskManager.hpp"

using namespace AnalysisTree;

void UserTaskWrite::Init() {
  auto man = TaskManager::GetInstance();
  auto chain = man->GetChain();

  AddInputBranch("SimParticles");
  particles_ = chain->GetBranch("SimParticles");

  auto br_conf = chain->GetConfiguration()->GetBranchConfig("SimParticles");
  auto new_conf = br_conf.Clone("NewParticles", DetType::kParticle);
  new_conf.AddField<float>("float_field", "just for test");
  new_conf.AddFields<int>({"int_field1", "int_field2"}, "just for test");

  new_particles_ = Branch(new_conf);
  new_particles_.SetMutable();
  man->AddBranch(&new_particles_);
}

void UserTaskWrite::Exec() {

  auto float_field = new_particles_.GetField("float_field");
  auto int_field = new_particles_.GetField("int_field1");

  for(size_t i=0; i<particles_.size(); ++i){
    auto particle = particles_[i];
    auto new_part = new_particles_.NewChannel();
    new_part.CopyContent(particle);

    new_part.SetValue(int_field, i);
    new_part.SetValue(float_field, cos(i));
  }
}

UserTaskWrite::~UserTaskWrite() {
  delete new_particles_ptr_;
}