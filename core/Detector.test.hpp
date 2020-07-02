#ifndef ANALYSISTREE_TEST_CORE_DETECTOR_TEST_HPP_
#define ANALYSISTREE_TEST_CORE_DETECTOR_TEST_HPP_

#include "Constants.hpp"
#include "Detector.hpp"

#include <TFile.h>
#include <TTree.h>
#include <gtest/gtest.h>
#include <vector>

namespace {

using namespace AnalysisTree;

TEST(Test_AnalysisTreeCore, Test_ChannelizedDetector) {
  ModuleDetector testChannelizedDetector;
  auto ch0 = testChannelizedDetector.AddChannel();
  ASSERT_EQ(ch0->GetId(), 0);
  ASSERT_EQ(testChannelizedDetector.GetNumberOfChannels(), 1);

  auto ch1 = testChannelizedDetector.AddChannel();
  ASSERT_FALSE(ch0 == ch1);
  ASSERT_EQ(ch1->GetId(), 1);
  ASSERT_EQ(testChannelizedDetector.GetNumberOfChannels(), 2);

  testChannelizedDetector.ClearChannels();
  ASSERT_EQ(testChannelizedDetector.GetNumberOfChannels(), 0);
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

  auto *testChannelizedDetector = new HitDetector;
  auto hitCh = testChannelizedDetector->AddChannel();
  hitCh->SetPosition(hitPosition);

  TFile outputFile("Test_WriteChDet.root", "recreate");
  TTree *chDetTree = new TTree("TestHitTree", "");
  chDetTree->Branch("testChannelizedDetector", &testChannelizedDetector);

  chDetTree->Fill();

  chDetTree->Write();
  outputFile.Close();

  HitDetector *newTestDetector = nullptr;
  TFile inputFile("Test_WriteChDet.root", "infra");
  chDetTree = (TTree *) inputFile.Get("TestHitTree");
  chDetTree->SetBranchAddress("testChannelizedDetector", &newTestDetector);
  chDetTree->GetEntry(0L);

  EXPECT_EQ(*newTestDetector, *testChannelizedDetector);
}

}// namespace

#endif//ANALYSISTREE_TEST_CORE_DETECTOR_TEST_HPP_
