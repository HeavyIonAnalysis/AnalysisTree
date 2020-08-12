#ifndef ANALYSISTREE_TEST_CORE_DETECTOR_TEST_HPP_
#define ANALYSISTREE_TEST_CORE_DETECTOR_TEST_HPP_

#include "Constants.hpp"
#include "Detector.hpp"

#include <TFile.h>
#include <TTree.h>
#include <gtest/gtest.h>

#include <vector>
#include <random>

namespace {

using namespace AnalysisTree;

template<class T>
DetType GetDetectorType(){}
template <> DetType GetDetectorType<Particle>() { return DetType::kParticle;}
template <> DetType GetDetectorType<Track>() { return DetType::kTrack;}
template <> DetType GetDetectorType<Hit>() { return DetType::kHit;}
template <> DetType GetDetectorType<Module>() { return DetType::kModule;}

template <class T>
class TestDetector : public Detector<T> {

 public:
  TestDetector() = default;
  ~TestDetector() override = default;

  void Init(){
    config_ = BranchConfig("TestDetector", GetDetectorType<T>());
    config_.AddField<float>("test_f");
    config_.AddField<int>("test_i");
    config_.AddField<bool>("test_b");
  }

  virtual void FillBaseInfo(T*) {};

  void FillDetector(size_t n_channels){
    this->ClearChannels();
    for(size_t i_channel=0; i_channel<n_channels; ++i_channel) {
      auto* ch = this->AddChannel();
      ch->Init(config_);

      FillBaseInfo(ch);

      const float f = std::rand() * (1. / RAND_MAX * 2.);
      const int i = std::rand() * (1. / RAND_MAX * 2.);
      const bool b = std::rand() * (1. / RAND_MAX) > 0.5;

      ch->SetField(f, 0);
      ch->SetField(i, 0);
      ch->SetField(b, 0);
    }
  }

  void WriteToFile(size_t n_events){

    TFile out_file("Test_WriteDetector.root", "recreate");
    auto *out_tree = new TTree("TestDetector", "");
    Detector<T>* pointer = this;
    out_tree->Branch("detector", &pointer);

    for(size_t i_event=0; i_event<n_events; ++i_event) {
      FillDetector(10);
      out_tree->Fill();
    }
    out_tree->Write();
    out_file.Close();
  }

 protected:
  BranchConfig config_;
};

class TestHitDetector : public TestDetector<Hit>{
 public:
  TestHitDetector() = default;
  ~TestHitDetector() override = default;

  void FillBaseInfo(Hit* hit) override {

    const float x = -1 + std::rand() * (1. / RAND_MAX * 2.);
    const float y = -1 + std::rand() * (1. / RAND_MAX * 2.);
    const float z = 2. + std::rand() * (1. / RAND_MAX);
    const float signal = std::rand() * (10. / RAND_MAX);

    hit->SetSignal(signal);
    hit->SetPosition({x, y, z});
  };
};


TEST(Test_AnalysisTreeCore, Test_WriteDetector) {

  TestHitDetector hit_detector;

  hit_detector.Init();
  hit_detector.WriteToFile(100);
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

  auto *hit = new Hit(1);
  hit->SetPosition(hitPosition);

  TFile outputFile("Test_WriteHit.root", "recreate");
  TTree *hitTree = new TTree("TestHitTree", "");
  hitTree->Branch("hit", &hit);

  hitTree->Fill();

  hitTree->Write();
  outputFile.Close();

  Hit *newHit = nullptr;
  TFile inputFile("Test_WriteHit.root", "infra");
  hitTree = (TTree *) inputFile.Get("TestHitTree");
  hitTree->SetBranchAddress("hit", &newHit);
  hitTree->GetEntry(0L);

  EXPECT_EQ(*newHit, *hit);
}

TEST(Test_AnalysisTreeCore, Test_WriteChannelizedDetector) {

  TVector3 hitPosition(1., 1., 1.);

  auto *module_detector = new HitDetector;
  auto hitCh = module_detector->AddChannel();
  hitCh->SetPosition(hitPosition);

  TFile outputFile("Test_WriteChDet.root", "recreate");
  TTree *chDetTree = new TTree("TestHitTree", "");
  chDetTree->Branch("module_detector", &module_detector);

  chDetTree->Fill();

  chDetTree->Write();
  outputFile.Close();

  HitDetector *newTestDetector = nullptr;
  TFile inputFile("Test_WriteChDet.root", "infra");
  chDetTree = (TTree *) inputFile.Get("TestHitTree");
  chDetTree->SetBranchAddress("module_detector", &newTestDetector);
  chDetTree->GetEntry(0L);

  EXPECT_EQ(*newTestDetector, *module_detector);
}

}// namespace

#endif//ANALYSISTREE_TEST_CORE_DETECTOR_TEST_HPP_
