#ifndef ANALYSISTREE_TEST_CORE_TRACK_TEST_HPP_
#define ANALYSISTREE_TEST_CORE_TRACK_TEST_HPP_

#include "TDatabasePDG.h"

#include <gtest/gtest.h>
#include <TFile.h>
#include <TTree.h>
#include <vector>

#include <core/Configuration.hpp>
#include <core/Detector.hpp>

namespace {

using namespace AnalysisTree;

TEST(Test_AnalysisTree, Test_TracksMomentum){

  Track track(0);

  const float px = std::rand() * (1./RAND_MAX*2.) ;
  const float py = std::rand() * (1./RAND_MAX*1.5);
  const float pz = 2. + std::rand() * (1./RAND_MAX);

  TLorentzVector vec;
  vec.SetVectM({px, py, pz}, 0.13957);

  track.SetMomentum(px, py, pz);

  ASSERT_FLOAT_EQ(track.GetPx(), px);
  ASSERT_FLOAT_EQ(track.GetPy(), py);
  ASSERT_FLOAT_EQ(track.GetPz(), pz);
  ASSERT_FLOAT_EQ(track.GetPt(),sqrt(px*px+py*py));
  ASSERT_FLOAT_EQ(track.GetPhi(), float(TMath::ATan2(py,px)));
  ASSERT_FLOAT_EQ(track.GetP(), sqrt(px*px+py*py+pz*pz));
  ASSERT_NEAR(track.GetRapidity(211), vec.Rapidity(), 1e-5);

  TLorentzVector vec1 = track.Get4Momentum(211);

  ASSERT_NEAR(vec1.Rapidity(), vec.Rapidity(), 1e-5);
  ASSERT_NEAR(vec1.M(), vec.M(), 1e-5);
}


TEST(Test_AnalysisTreeCore, Test_WriteTracks) {

  TFile *f = TFile::Open("test.root", "recreate");
  TTree *t{new TTree("test", "")};

  Configuration config;

  AnalysisTree::BranchConfig RecTracksBranch("RecTrack", AnalysisTree::DetType::kTrack);
  RecTracksBranch.AddField<float>("dcax");
  RecTracksBranch.AddField<float>("dcay");
  RecTracksBranch.AddField<float>("dcaz");
  RecTracksBranch.AddField<int>("nhits");

  config.AddBranchConfig(RecTracksBranch);
  auto *RecTracks = new AnalysisTree::TrackDetector(0);

  EXPECT_EQ(RecTracksBranch.GetFieldId("dcax"), 0);
  EXPECT_EQ(RecTracksBranch.GetFieldId("dcay"), 1);
  EXPECT_EQ(RecTracksBranch.GetFieldId("nhits"), 0);
  EXPECT_EQ(RecTracksBranch.GetFieldId("pT"), TrackFields::kPt);
  EXPECT_EQ(RecTracksBranch.GetFieldId("phi"), TrackFields::kPhi);
  EXPECT_EQ(RecTracksBranch.GetFieldId("eta"), TrackFields::kEta);
  EXPECT_EQ(RecTracksBranch.GetFieldId("p"), TrackFields::kP);
  EXPECT_EQ(RecTracksBranch.GetFieldId("px"), TrackFields::kPx);
  EXPECT_EQ(RecTracksBranch.GetFieldId("py"), TrackFields::kPy);
  EXPECT_EQ(RecTracksBranch.GetFieldId("pz"), TrackFields::kPz);

  t->Branch("RecTracks", "AnalysisTree::TrackDetector", &RecTracks);

  for(int i=0; i<10; ++i) {
    RecTracks->ClearChannels();
    int n_tracks = 5;//std::rand() % 100;
    for (int j=0; j<n_tracks; ++j) {
      auto* iTrack = RecTracks->AddChannel();
      iTrack->Init(RecTracksBranch);

      const float px = std::rand() * (1./RAND_MAX*2.) ;
      const float py = std::rand() * (1./RAND_MAX*1.5);
      const float pz = 2. + std::rand() * (1./RAND_MAX);

      iTrack->SetMomentum(px,py,pz);
      iTrack->SetField(1.f, RecTracksBranch.GetFieldId("dcax"));
      iTrack->SetField(2.f, RecTracksBranch.GetFieldId("dcay"));
      iTrack->SetField(3.f, RecTracksBranch.GetFieldId("dcaz"));
      iTrack->SetField(4, RecTracksBranch.GetFieldId("nhits"));
    }
    t->Fill();
  }

  config.Write("Configuration");
  t->Write();
  f->Close();
}

}


#endif //ANALYSISTREE_TEST_CORE_TRACK_TEST_HPP_
