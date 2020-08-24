#ifndef ANALYSISTREE_TEST_CORE_DETECTOR_TEST_HPP_
#define ANALYSISTREE_TEST_CORE_DETECTOR_TEST_HPP_

#include "Constants.hpp"
#include "Detector.hpp"

#include <TFile.h>
#include <TTree.h>
#include <gtest/gtest.h>

#include <random>
#include <vector>

namespace {

using namespace AnalysisTree;

template<class T>
DetType GetDetectorType() {}
template<>
DetType GetDetectorType<Particle>() { return DetType::kParticle; }
template<>
DetType GetDetectorType<Track>() { return DetType::kTrack; }
template<>
DetType GetDetectorType<Hit>() { return DetType::kHit; }
template<>
DetType GetDetectorType<Module>() { return DetType::kModule; }

template<class T, class GENERATOR>
class TestDetector : public Detector<T> {

 public:
  TestDetector() = default;
  ~TestDetector() override = default;

  void Init() {
    config_ = BranchConfig("TestDetector", GetDetectorType<T>());
    config_.AddField<float>("test_f");
    config_.AddField<int>("test_i");
    config_.AddField<bool>("test_b");
  }

  virtual void FillBaseInfo(T*){};

  void FillDetector(size_t n_channels) {
    this->ClearChannels();
    for (size_t i_channel = 0; i_channel < n_channels; ++i_channel) {
      auto* ch = this->AddChannel();
      ch->Init(config_);

      FillBaseInfo(ch);

      const float f = random_f_(generator_);
      const int i = random_i_(generator_);
      const bool b = random_b_(generator_);

      ch->SetField(f, 0);
      ch->SetField(i, 0);
      ch->SetField(b, 0);
    }
  }
 protected:
  BranchConfig config_;
  GENERATOR generator_;
  std::uniform_real_distribution<float> random_f_{-1, 1};
  std::uniform_int_distribution<int> random_i_{-1000, 1000};
  std::uniform_int_distribution<int> random_b_{0, 1};
};

template<class GENERATOR>
class TestParticles : public TestDetector<Particle, GENERATOR> {
 public:
  TestParticles() = default;
  ~TestParticles() override = default;

  void FillBaseInfo(Particle* particle) override {

    const float phi = phi_distr_(this->generator_);
    const float pT = pT_dist_(this->generator_);
    const float eta = eta_dist_(this->generator_);
    const int pdg = 211;

    TVector3 mom;
    mom.SetPtEtaPhi(pT, eta, phi);
    particle->SetMomentum3(mom);
    particle->SetPid(pdg);
  };

 private:
  std::uniform_real_distribution<float> phi_distr_{-M_PI, M_PI};
  std::exponential_distribution<float> pT_dist_{1.};
  std::normal_distribution<float> eta_dist_{0., 1.};
};

template<class GENERATOR>
class TestTrackDetector : public TestDetector<Track, GENERATOR> {
 public:
  TestTrackDetector() = default;
  ~TestTrackDetector() override = default;

  void FillBaseInfo(Track* track) override {

    const float phi = phi_distr_(this->generator_);
    const float pT = pT_dist_(this->generator_);
    const float eta = eta_dist_(this->generator_);

    TVector3 mom;
    mom.SetPtEtaPhi(pT, eta, phi);
    track->SetMomentum3(mom);
  };

  void FillDetector(const TestParticles<GENERATOR>& particles){
    for(const auto& particle : particles.GetChannels()){
      bool is_passed = eff_(this->generator_) > 50;
      if(!is_passed) continue;

      auto* track = this->AddChannel();
      track->Init(this->config_);
      track = particle;
    }
  }

 private:
  std::uniform_real_distribution<float> phi_distr_{-M_PI, M_PI};
  std::exponential_distribution<float> pT_dist_{1.};
  std::normal_distribution<float> eta_dist_{0., 1.};

  std::uniform_int_distribution<int> eff_{0, 100};

};


TEST(Test_AnalysisTreeCore, Test_WriteDetector) {

  TestParticles<std::default_random_engine> particle;

  particle.Init();
//  particle.WriteToFile(100);
}

TEST(Test_AnalysisTreeCore, Test_ChannelizedDetector) {
  ModuleDetector module_detector;
  auto ch0 = module_detector.AddChannel();
  ASSERT_EQ(ch0->GetId(), 0);
  ASSERT_EQ(module_detector.GetNumberOfChannels(), 1);

  auto ch1 = module_detector.AddChannel();
  ASSERT_FALSE(ch0 == ch1);
  ASSERT_EQ(ch1->GetId(), 1);
  ASSERT_EQ(module_detector.GetNumberOfChannels(), 2);

  module_detector.ClearChannels();
  ASSERT_EQ(module_detector.GetNumberOfChannels(), 0);
}

TEST(Test_AnalysisTreeCore, Test_WriteHit) {
  TVector3 hitPosition(1, 1, 1);

  auto* hit = new Hit(1);
  hit->SetPosition(hitPosition);

  TFile outputFile("Test_WriteHit.root", "recreate");
  auto* hitTree = new TTree("TestHitTree", "");
  hitTree->Branch("hit", &hit);

  hitTree->Fill();

  hitTree->Write();
  outputFile.Close();

  Hit* newHit = nullptr;
  TFile inputFile("Test_WriteHit.root", "infra");
  hitTree = (TTree*) inputFile.Get("TestHitTree");
  hitTree->SetBranchAddress("hit", &newHit);
  hitTree->GetEntry(0L);

  EXPECT_EQ(*newHit, *hit);
}

TEST(Test_AnalysisTreeCore, Test_WriteChannelizedDetector) {

  TVector3 hitPosition(1., 1., 1.);

  auto* module_detector = new HitDetector;
  auto hitCh = module_detector->AddChannel();
  hitCh->SetPosition(hitPosition);

  TFile outputFile("Test_WriteChDet.root", "recreate");
  TTree* chDetTree = new TTree("TestHitTree", "");
  chDetTree->Branch("module_detector", &module_detector);

  chDetTree->Fill();

  chDetTree->Write();
  outputFile.Close();

  HitDetector* newTestDetector = nullptr;
  TFile inputFile("Test_WriteChDet.root", "infra");
  chDetTree = (TTree*) inputFile.Get("TestHitTree");
  chDetTree->SetBranchAddress("module_detector", &newTestDetector);
  chDetTree->GetEntry(0L);

  EXPECT_EQ(*newTestDetector, *module_detector);
}

}// namespace

#endif//ANALYSISTREE_TEST_CORE_DETECTOR_TEST_HPP_
