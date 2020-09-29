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

      data_vtx_tracks->ClearChannels();
      for (size_t iTr = 0; iTr < 10; ++iTr) {
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
    EXPECT_EQ(atb_vtx.GetNumberOfChannels(), 10);
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
    define_result->SetEntry(i);
    EXPECT_EQ(define_result->GetDataMatrix()["const"][0], 2.);
  }
  define_result->SetEntry(1);
  std::cout << *define_result;

  EXPECT_EQ(atb.Apply(BranchViewAction::NewDefineAction("double_const", {}, [] () -> double { return 1.0; }))->GetFieldPtr("double_const")->GetFieldTypeStr(), "double");
  EXPECT_EQ(atb.Apply(BranchViewAction::NewDefineAction("int_const", {}, [] () -> int { return 1; }))->GetFieldPtr("int_const")->GetFieldTypeStr(), "int");
  EXPECT_EQ(atb.Apply(BranchViewAction::NewDefineAction("float_const", {}, [] () -> float { return 1; }))->GetFieldPtr("float_const")->GetFieldTypeStr(), "float");
  EXPECT_EQ(atb.Apply(BranchViewAction::NewDefineAction("bool_const", {}, [] () -> bool { return true; }))->GetFieldPtr("bool_const")->GetFieldTypeStr(), "bool");
}

TEST(Test_BranchViewAction, Filter) {
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

      data_vtx_tracks->ClearChannels();
      for (size_t iTr = 0; iTr < 10; ++iTr) {
        auto channel = data_vtx_tracks->AddChannel();
        channel->Init(vtx_tracks_config);
        float px = iTr < 2? 0. : 1.;
        channel->SetMomentum(px, 20.*iEv, 30.*iEv);
      } // tracks
      tree->Fill();
    }
    tree->Write();
    f.Close();
  }


  TFile f("tmp.root", "READ");
  AnalysisTreeBranch<EventHeader> atb(event_header_config, f.Get<TTree>("aTree"));

  EXPECT_NO_THROW(atb.Apply(BranchViewAction::NewFilterAction({}, [] () -> bool {return false;})));
  EXPECT_THROW(atb.Apply(BranchViewAction::NewFilterAction({}, [] () -> double {return false;})), std::runtime_error);

  /* none passing */
  auto filter_false = atb.Apply(BranchViewAction::NewFilterAction({}, [] () -> bool {return false;}));
  filter_false->SetEntry(0);
  EXPECT_EQ(filter_false->GetNumberOfChannels(), 0);
  /* all passing */
  auto filter_true = atb.Apply(BranchViewAction::NewFilterAction({}, [] () -> bool {return true;}));
  filter_true->SetEntry(0);
  EXPECT_EQ(filter_true->GetNumberOfChannels(), 1);

  AnalysisTreeBranch<TrackDetector> atb_vtx(vtx_tracks_config, f.Get<TTree>("aTree"));
  auto filter_px = atb_vtx.Apply(BranchViewAction::NewFilterAction({"px"}, [] (double px) -> bool {return px == 0.;}));
  for (Long64_t iEntry = 0; iEntry < atb_vtx.GetEntries(); ++iEntry) {
    filter_px->SetEntry(iEntry);
    atb_vtx.SetEntry(iEntry);
    EXPECT_EQ(atb_vtx.GetNumberOfChannels(), 10);
    EXPECT_EQ(filter_px->GetNumberOfChannels(), 2);
  }
  filter_px->SetEntry(1);
  std::cout << *filter_px;
}

}

