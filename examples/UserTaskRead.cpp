#include "UserTaskRead.hpp"

#include <AnalysisTree/TaskManager.hpp>

using namespace AnalysisTree;

void UserTaskRead::Init() {

  auto chain = TaskManager::GetInstance()->GetChain();

  in_branches_.emplace("SimParticles");
  in_branches_.emplace("RecTracks");

  particles_ = chain->GetBranch("SimParticles");
  tracks_ = chain->GetBranch("RecTracks");
}

void UserTaskRead::Exec() {
  std::cout << particles_.size() << std::endl;
  std::cout << tracks_.size() << std::endl;

  auto pT = particles_.GetField("pT");
  for (size_t i = 0; i < particles_.size(); ++i) {
    auto particle = particles_[i];
    std::cout << particle[pT] << std::endl;
  }
}

UserTaskRead::~UserTaskRead() = default;