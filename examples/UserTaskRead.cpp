#include "UserTaskRead.hpp"

#include <TaskManager.hpp>

using namespace AnalysisTree;

void UserTaskRead::Init() {

  auto chain = TaskManager::GetInstance()->GetChain();

  AddInputBranch("SimParticles");
  AddInputBranch("RecTracks");

  particles_ = chain->GetBranch("SimParticles");
  tracks_ = chain->GetBranch("RecTracks");
  match_ = chain->GetMatching("RecTracks", "SimParticles");
}

void UserTaskRead::Exec() {
  std::cout << particles_.size() << std::endl;
  std::cout << tracks_.size() << std::endl;

  auto sim_pT = particles_.GetField("pT");
  auto rec_pT = tracks_.GetField("pT");

  for (size_t i = 0; i < tracks_.size(); ++i) {
    auto track = tracks_[i];
    auto match_id = match_->GetMatch(track.GetId());
    std::cout << "rec track # " << i << " pT = " << track[rec_pT] << std::endl;

    if(match_id >= 0){
      auto particle = particles_[match_id];
      std::cout << "matched sim track # " << match_id << " pT = " << particle[sim_pT] << std::endl;
    }
  }
}

UserTaskRead::~UserTaskRead() = default;