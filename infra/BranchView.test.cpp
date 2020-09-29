//
// Created by eugene on 25/09/2020.
//

#include <TFile.h>
#include <AnalysisTree/EventHeader.hpp>
#include <AnalysisTree/BranchView.hpp>
#include <AnalysisTree/BranchConfig.hpp>
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
    atb.SetEntry(iEv);
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
  EXPECT_NO_THROW(atb.Apply(BranchViewAction::NewDefineAction("vtx_xy", {"vtx_x", "vtx_y"}, [] (double , double ) -> double { return 1.0; })));
  EXPECT_THROW(atb.Apply(BranchViewAction::NewDefineAction("vtx_x", {"vtx_x", "vtx_y"}, [] (double , double ) -> double { return 1.0; })), std::runtime_error);
  EXPECT_THROW(atb.Apply(BranchViewAction::NewDefineAction("vtx_xy", {"vtx_x", "tx_y"}, [] (double , double ) -> double { return 1.0; })), std::out_of_range);
  EXPECT_EQ(atb.Apply(BranchViewAction::NewDefineAction("vtx_xy", {"vtx_x", "vtx_y"}, [] (double , double ) -> double { return 1.0; }))->GetFields(), std::vector<std::string>({"vtx_xy","vtx_x","vtx_y","vtx_z"}));


  auto define_result = atb.Apply(BranchViewAction::NewDefineAction("const", {}, [] () -> double { return 2.0; }));
  for (size_t i = 0; i < 100; ++i) {
    atb.SetEntry(i);
    EXPECT_EQ(define_result->GetDataMatrix()["const"][0], 2.);
  }

  EXPECT_EQ(atb.Apply(BranchViewAction::NewDefineAction("double_const", {}, [] () -> double { return 1.0; }))->GetFieldPtr("double_const")->GetFieldTypeStr(), "double");
  EXPECT_EQ(atb.Apply(BranchViewAction::NewDefineAction("int_const", {}, [] () -> int { return 1; }))->GetFieldPtr("int_const")->GetFieldTypeStr(), "int");
  EXPECT_EQ(atb.Apply(BranchViewAction::NewDefineAction("float_const", {}, [] () -> float { return 1; }))->GetFieldPtr("float_const")->GetFieldTypeStr(), "float");
  EXPECT_EQ(atb.Apply(BranchViewAction::NewDefineAction("bool_const", {}, [] () -> bool { return true; }))->GetFieldPtr("bool_const")->GetFieldTypeStr(), "bool");

}


}

