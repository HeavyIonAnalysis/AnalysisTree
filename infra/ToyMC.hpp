#ifndef ANALYSISTREE_INFRA_TOYMC_HPP_
#define ANALYSISTREE_INFRA_TOYMC_HPP_

#include <TH2.h>
#include <fstream>
#include <random>

#include "TTree.h"
#include "TFile.h"
#include "TF1.h"

#include "Configuration.hpp"
#include "DataHeader.hpp"
#include "EventHeader.hpp"
#include "Matching.hpp"

#include "Task.hpp"
#include "TaskManager.hpp"

namespace AnalysisTree{

template<class RandomEngine>
class ToyMC : public Task {
 public:
  ToyMC() = default;
  ~ToyMC() override = default;

  void Init() override {

    auto* man = TaskManager::GetInstance();

    BranchConfig sim_eh("SimEventHeader", DetType::kEventHeader);
    sim_eh.AddField<float>("psi_RP");

    BranchConfig sim_part("SimParticles", DetType::kParticle);
    sim_part.AddField<float>("float");
    sim_part.AddField<bool>("bool");
    sim_part.AddField<int>("int");

    man->AddBranch("SimEventHeader", sim_event_header_, sim_eh);
    man->AddBranch("SimParticles", particles_, sim_part);
    man->AddBranch("RecTracks", track_detector_, BranchConfig{"RecTracks", DetType::kTrack});
    man->AddMatching("RecTracks", "SimParticles", &rec_tracks_to_sim_);

    sim_event_header_->Init(sim_eh);

  }

  void Exec() override {
    FillEventInfo();
    FillMcParticles();
    FillRecoTracks();
  }

  void Finish() override {

  }


  void FillEventInfo(){
    sim_event_header_->SetVertexPosition3({0,0,0});
    sim_event_header_->SetField(psi_RP_(generator_), 0);
  }

  void FillMcParticles(){

    particles_->ClearChannels();

    const auto& branch = config_->GetBranchConfig("SimParticles");
    const int multiplicity = multiplicity_(generator_);
    particles_->Reserve(multiplicity);

    for(int i=0; i<multiplicity; ++i){

      const float phi = GetPhi(generator_, sim_event_header_->GetField<float>(0));
      const float pT = pT_dist_(generator_);
      const float eta = y_dist_(generator_);
      const int pdg = 211;

      TVector3 mom;
      mom.SetPtEtaPhi(pT, eta, phi);

      auto& particle = particles_->AddChannel(branch);

      particle.SetMomentum3(mom);
      particle.SetPid(pdg);
    }
  }

  void FillRecoTracks(){
    assert(particles_);
//    assert(efficiency_map_);

    track_detector_->ClearChannels();
    rec_tracks_to_sim_->Clear();

    track_detector_->Reserve(particles_->GetNumberOfChannels());
    const auto& branch = config_->GetBranchConfig("RecTracks");

    for(const auto& particle : *particles_){
      auto& track = track_detector_->AddChannel(branch);
      track = particle;
      rec_tracks_to_sim_->AddMatch(track.GetId(), particle.GetId());
    }
  }

  void WriteToFile(const std::string& filename, const std::string& filelist="") const {
//    TFile* file{TFile::Open(filename.c_str(), "recreate")};
//    config_.Write("Configuration");
//    dh_.Write("DataHeader");
//    out_tree_->Write();
//    file->Close();
//    if(!filelist.empty()){
//      std::ofstream fl(filelist);
//      fl << filename << "\n";
//      fl.close();
//    }
  }

 protected:
  RandomEngine generator_;

  //event properties
  EventHeader* sim_event_header_{nullptr};
  std::poisson_distribution<int> multiplicity_{10000};
  std::uniform_real_distribution<float> psi_RP_{-M_PI, M_PI};
  std::vector<float> vn_{0.2, 0.1};
  float cm_rapidity_{0.f};

  // particle properties
  Particles* particles_{nullptr};
  std::exponential_distribution<float> pT_dist_{1.};
  std::normal_distribution<float> y_dist_{cm_rapidity_, 1.};
  std::piecewise_linear_distribution<> phi_distr_{1000, 0, 2*M_PI, [&](const double x) { return PhiPdf(x); }};

  // tracking detector properties
  TrackDetector* track_detector_{nullptr};
  Matching* rec_tracks_to_sim_{nullptr};

  TH2* efficiency_map_{nullptr};

  float GetPhi(RandomEngine &engine, float psi) {
    return phi_distr_(engine) + psi;
  }

  double PhiPdf(double phi) {
    double value = 1.;
    for (unsigned int n=1; n < vn_.size()+1; ++n) {
      value += 2 * vn_[n-1] * std::cos(n * (phi - 0.));
    }
    return value;
  }

};


}

#endif //ANALYSISTREE_INFRA_TOYMC_HPP_
