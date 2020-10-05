//
// Created by eugene on 25/09/2020.
//

#include <AnalysisTree/BranchConfig.hpp>
#include <AnalysisTree/BranchView.hpp>
#include <AnalysisTree/EventHeader.hpp>
#include <TFile.h>
#include <gtest/gtest.h>

namespace {

using namespace AnalysisTree;

TEST(Test_AnalysisTreeBranch, Test_GetFields) {
  BranchConfig c("test", DetType::kEventHeader);
  AnalysisTreeBranch<EventHeader> br(c);

  EXPECT_EQ(br.GetFields(), std::vector<std::string>({"vtx_x", "vtx_y", "vtx_z"}));
}

TEST(Test_AnalysisTreeBranch, Test_GetNChannels) {
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
        channel->SetMomentum(10. * iEv, 20. * iEv, 30. * iEv);
      }

      tree->Fill();
    }
    tree->Write();
    f.Close();
  }

  AnalysisTreeBranch<EventHeader> atb(event_header_config, "aTree", "tmp.root");
  AnalysisTreeBranch<TrackDetector> atb_vtx(vtx_tracks_config, "aTree", "tmp.root");

  for (size_t iEv = 0; iEv < 100; ++iEv) {
    atb.SetEntry(iEv);
    atb_vtx.SetEntry(iEv);
    auto results = atb.GetDataMatrix();
    EXPECT_EQ(results.size(), atb.GetFields().size());
    EXPECT_EQ(results["vtx_x"].size(), atb.GetNumberOfChannels());
    EXPECT_EQ(results["vtx_x"][0], 10 * iEv + 1);
    EXPECT_EQ(results["vtx_y"][0], 10 * iEv + 2);
    EXPECT_EQ(results["vtx_z"][0], 10 * iEv + 3);

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

  AnalysisTreeBranch<EventHeader> atb(event_header_config, "aTree", "tmp.root");
  EXPECT_NO_THROW(atb.Define("vtx_xy", {"vtx_x", "vtx_y"}, [](double, double) -> double { return 1.0; }));
  EXPECT_THROW(atb.Define("vtx_x", {"vtx_x", "vtx_y"}, [](double, double) -> double { return 1.0; }), std::runtime_error);
  EXPECT_THROW(atb.Define("vtx_xy", {"vtx_x", "tx_y"}, [](double, double) -> double { return 1.0; }), std::out_of_range);
  EXPECT_EQ(atb.Define("vtx_xy", {"vtx_x", "vtx_y"}, [](double, double) -> double { return 1.0; })->GetFields(), std::vector<std::string>({"vtx_xy", "vtx_x", "vtx_y", "vtx_z"}));

  auto define_result = atb.Define("const", {}, []() -> double { return 2.0; });
  for (size_t i = 0; i < 100; ++i) {
    define_result->SetEntry(i);
    EXPECT_EQ(define_result->GetDataMatrix()["const"][0], 2.);
  }
  define_result->SetEntry(1);
  std::cout << *define_result;

  EXPECT_EQ(atb.Define("double_const", {}, []() -> double { return 1.0; })->GetFieldPtr("double_const")->GetFieldTypeStr(), "double");
  EXPECT_EQ(atb.Define("int_const", {}, []() -> int { return 1; })->GetFieldPtr("int_const")->GetFieldTypeStr(), "int");
  EXPECT_EQ(atb.Define("float_const", {}, []() -> float { return 1; })->GetFieldPtr("float_const")->GetFieldTypeStr(), "float");
  EXPECT_EQ(atb.Define("bool_const", {}, []() -> bool { return true; })->GetFieldPtr("bool_const")->GetFieldTypeStr(), "bool");
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
        float px = iTr < 2 ? 0. : 1.;
        channel->SetMomentum(px, 20. * iEv, 30. * iEv);
      }// tracks
      tree->Fill();
    }
    tree->Write();
    f.Close();
  }

  TFile f("tmp.root", "READ");
  AnalysisTreeBranch<EventHeader> atb(event_header_config, "aTree", "tmp.root");

  EXPECT_NO_THROW(atb.Filter({}, []() -> bool { return false; }));
  EXPECT_THROW(atb.Filter({}, []() -> double { return false; }), std::runtime_error);

  /* none passing */
  auto filter_false = atb.Filter({}, []() -> bool { return false; });
  filter_false->SetEntry(0);
  EXPECT_EQ(filter_false->GetNumberOfChannels(), 0);
  /* all passing */
  auto filter_true = atb.Filter({}, []() -> bool { return true; });
  filter_true->SetEntry(0);
  EXPECT_EQ(filter_true->GetNumberOfChannels(), 1);

  AnalysisTreeBranch<TrackDetector> atb_vtx(vtx_tracks_config, "aTree", "tmp.root");
  auto filter_px = atb_vtx.Filter({"px"}, [](double px) -> bool { return px == 0.; });
  for (Long64_t iEntry = 0; iEntry < atb_vtx.GetEntries(); ++iEntry) {
    filter_px->SetEntry(iEntry);
    atb_vtx.SetEntry(iEntry);
    EXPECT_EQ(atb_vtx.GetNumberOfChannels(), 10);
    EXPECT_EQ(filter_px->GetNumberOfChannels(), 2);
  }
  filter_px->SetEntry(1);
  std::cout << *filter_px;
}

TEST(Test_BranchViewAction, Rename) {
  BranchConfig c("test", DetType::kEventHeader);
  AnalysisTreeBranch<EventHeader> br(c);

  EXPECT_NO_THROW(br.RenameFields({{"vtx_x", "vtx_y"}, {"vtx_y", "vtx_x"}})); /* exchange fields */
  EXPECT_NO_THROW(br.RenameFields("vtx_x", "vtx_x"));                         /* no change */
  EXPECT_THROW(br.RenameFields("no-field", "VTX_X"), std::out_of_range);
  EXPECT_THROW(br.RenameFields({{"vtx_x", "VTX_XY"}, {"xtx_y", "VTX_XY"}}), std::runtime_error);
  EXPECT_TRUE(br.RenameFields("vtx_x", "VTX_X")->HasField("VTX_X"));
  EXPECT_FALSE(br.RenameFields("vtx_x", "VTX_X")->HasField("vtx_x"));
  EXPECT_THROW(br.RenameFields("vtx_x", "VTX_X")->GetFieldPtr("vtx_x"), std::out_of_range);
  EXPECT_NO_THROW(br.RenameFields("vtx_x", "VTX_X")->GetFieldPtr("VTX_X"));

  std::cout << br << std::endl;
  std::cout << *br.RenameFields("vtx_x", "VTX_X") << std::endl;

  EXPECT_EQ(br.AddPrefix("sim_")->GetFields(), std::vector<std::string>({"sim_vtx_x", "sim_vtx_y", "sim_vtx_z"}));
}

TEST(Test_BranchViewAction, Merge) {

  BranchConfig rec_header_config("rec_event", DetType::kEventHeader);
  AnalysisTreeBranch<EventHeader> rec_header_branch(rec_header_config);
  BranchConfig sim_header_config("sim_event", DetType::kEventHeader);
  AnalysisTreeBranch<EventHeader> sim_header_branch(sim_header_config);

  EXPECT_THROW(rec_header_branch.Merge(sim_header_branch), std::runtime_error);// same vtx_x, vtx_y, vtx_z
  EXPECT_NO_THROW(rec_header_branch.Merge(sim_header_branch, "sim_", "rec_"));
  auto merge_result = rec_header_branch.Merge(sim_header_branch, "rec_", "sim_");
  EXPECT_EQ(merge_result->GetNumberOfChannels(), 1);
  merge_result->SetEntry(0);
  std::cout << *merge_result << std::endl;

  auto* vtx_tracks = new TrackDetector;
  BranchConfig vtx_tracks_config("vtx_tracks", DetType::kTrack);
  AnalysisTreeBranch<TrackDetector> vtx_tracks_branch(vtx_tracks_config, vtx_tracks);
  EXPECT_EQ(vtx_tracks_branch.GetNumberOfChannels(), 0);
  EXPECT_EQ(rec_header_branch.Merge(vtx_tracks_branch)->GetNumberOfChannels(), 0);
  EXPECT_EQ(vtx_tracks_branch.Merge(rec_header_branch)->GetNumberOfChannels(), 0);

  /* add few channels */
  for (size_t i = 0; i < 10; ++i) {
    auto* ch = vtx_tracks->AddChannel();
    ch->SetMomentum(i, i, i);
  }

  std::cout << vtx_tracks_branch << std::endl;
  merge_result = vtx_tracks_branch.Merge(rec_header_branch);
  merge_result->SetEntry(0);
  std::cout << *(merge_result) << std::endl;

  auto* sim_tracks = new TrackDetector;
  BranchConfig sim_tracks_config("sim_tracks", DetType::kTrack);
  AnalysisTreeBranch<TrackDetector> sim_tracks_branch(sim_tracks_config, sim_tracks);

  /* add few channels */
  for (size_t i = 0; i < 10; ++i) {
    auto* ch = sim_tracks->AddChannel();
    ch->SetMomentum(-float(i), -float(i), -float(i));
  }

  merge_result = vtx_tracks_branch.Merge(sim_tracks_branch, "rec_", "sim_");
  merge_result->SetEntry(0);
  std::cout << *(merge_result) << std::endl;


}
}// namespace
