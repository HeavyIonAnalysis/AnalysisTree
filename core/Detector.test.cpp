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

TEST(Detector, ChannelizedDetector) {
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

TEST(Detector, WriteHit) {
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

TEST(Detector, WriteChannelizedDetector) {

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
