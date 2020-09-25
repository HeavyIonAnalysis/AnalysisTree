//
// Created by eugene on 25/09/2020.
//

#ifndef ANALYSISTREE_INFRA_BRANCHVIEW_TEST_HPP
#define ANALYSISTREE_INFRA_BRANCHVIEW_TEST_HPP

#include "BranchView.hpp"
#include "BranchConfig.hpp"
#include <gtest/gtest.h>

namespace {

using namespace AnalysisTree;


TEST(Test_AnalysisTreeBranch,Test_GetFields) {
  BranchConfig c("test", DetType::kEventHeader);
  AnalysisTreeBranch<EventHeader> br(c);

  EXPECT_EQ(br.GetFields(), std::vector<std::string>({"vtx_x","vtx_y","vtx_z"}));
}

TEST(Test_AnalysisTreeBranch,Test_GetNChannels) {
  BranchConfig c("test", DetType::kEventHeader);
  AnalysisTreeBranch<EventHeader> br(c);
  EXPECT_EQ(br.GetNumberOfChannels(), 1);

  BranchConfig c1("test", DetType::kTrack);
  AnalysisTreeBranch<TrackDetector> br1(c);
  EXPECT_EQ(br1.GetNumberOfChannels(), 0);
}

TEST(Test_AnalysisTreeBranch, Test_GetDataMatrix) {
  BranchConfig event_header_config("test", DetType::kEventHeader);
  auto data_event_header = new EventHeader;

  BranchConfig vtx_tracks_config("vtx_tracks", DetType::kTrack);
  auto data_vtx_tracks = new TrackDetector;

  {
    TFile f("tmp.root", "RECREATE");
    auto tree = new TTree("aTree", "");
    tree->Branch(event_header_config.GetName().c_str(), &data_event_header);
    tree->Branch(vtx_tracks_config.GetName().c_str(), &data_vtx_tracks);

    data_event_header->Init(event_header_config);
    for (size_t iEv = 0; iEv < 100; ++iEv) {
      data_event_header->SetVertexPosition3({float(10 * iEv + 1), float(10 * iEv + 2), float(10 * iEv + 3)});

      for (size_t iTr = 0; iTr < 10*iEv; ++iTr) {
        auto channel = data_vtx_tracks->AddChannel();
        channel->Init(vtx_tracks_config);
        channel->SetMomentum(10.*iEv, 20.*iEv, 30.*iEv);
      }

      tree->Fill();
    }
    tree->Write();
    f.Close();
  }

  TFile f("tmp.root", "READ");
  AnalysisTreeBranch<EventHeader> atb(event_header_config, f.Get<TTree>("aTree"));
  AnalysisTreeBranch<TrackDetector> atb_vtx(vtx_tracks_config, f.Get<TTree>("aTree"));

  for (size_t iEv = 0; iEv < 100; ++iEv) {
    atb.GetEntry(iEv);
    auto results = atb.GetDataMatrix();
    EXPECT_EQ(results.size(), atb.GetFields().size());
    EXPECT_EQ(results["vtx_x"].size(), atb.GetNumberOfChannels());
    EXPECT_EQ(results["vtx_x"][0], 10*iEv + 1);
    EXPECT_EQ(results["vtx_y"][0], 10*iEv + 2);
    EXPECT_EQ(results["vtx_z"][0], 10*iEv + 3);

    auto vtx_tracks_results = atb_vtx.GetDataMatrix();
    EXPECT_EQ(vtx_tracks_results.size(), atb_vtx.GetFields().size());
    EXPECT_EQ(vtx_tracks_results["px"].size(), atb_vtx.GetNumberOfChannels());
  }
}

TEST(Test_BranchViewAction, Define) {
  BranchConfig event_header_config("test", DetType::kEventHeader);
  auto data_event_header = new EventHeader;

  {
    TFile f("tmp.root", "RECREATE");
    auto tree = new TTree("aTree", "");
    tree->Branch(event_header_config.GetName().c_str(), &data_event_header);

    data_event_header->Init(event_header_config);
    for (size_t iEv = 0; iEv < 100; ++iEv) {
      data_event_header->SetVertexPosition3({float(10 * iEv + 1), float(10 * iEv + 2), float(10 * iEv + 3)});
      tree->Fill();
    }
    tree->Write();
    f.Close();
  }


  TFile f("tmp.root", "READ");
  AnalysisTreeBranch<EventHeader> atb(event_header_config, f.Get<TTree>("aTree"));
  EXPECT_NO_THROW(atb.Define("vtx_xy", std::vector<std::string>({"vtx_x", "vtx_y"}), [] () {}));
}


}

#endif//ANALYSISTREE_INFRA_BRANCHVIEW_TEST_HPP
