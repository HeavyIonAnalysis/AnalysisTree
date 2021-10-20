//
// Created by eugene on 17/10/2021.
//

#include "AnalysisTreeRDF.hpp"

#include <iostream>
#include <gtest/gtest.h>

#include <ROOT/RDataFrame.hxx>

using namespace AnalysisTree;

void makeTestTree(int ne = 5) {

  TFile f("test.root", "recreate");


  BranchConfig rec_event_header("RecEvent", DetType::kEventHeader);
  rec_event_header.AddField<int>("int1", "");

  BranchConfig vtx_tracks_branch("VtxTracks", DetType::kTrack);

  auto event = new EventHeader;
  event->Init(rec_event_header);

  auto vtx_tracks = new TrackDetector;

  auto tree = new TTree("aTree", "Test tree");
  tree->Branch("RecEvent", &event);
  tree->Branch("VtxTracks", &vtx_tracks);

  for (int i = 0; i < ne; ++i) {
    event->Init(rec_event_header);
    event->SetVertexPosition3(TVector3(i, 10*i, 100*i));

    vtx_tracks->ClearChannels();
    for (int j = 0; j < 10; ++j) {
      vtx_tracks->AddChannel(vtx_tracks_branch);
      vtx_tracks->Channel(j).SetMomentum(1.0*j, 10.*j, 100.*j);
    }

    tree->Fill();
  }

  tree->Write();

  Configuration at_config;
  at_config.AddBranchConfig(rec_event_header);
  at_config.AddBranchConfig(vtx_tracks_branch);
  f.WriteObject(&at_config, "Configuration");

  f.Close();
}

TEST(AnalysisTreeRDF, Display) {
  makeTestTree(25);

  auto ds = std::make_unique<Impl::AnalysisTreeRDFImplT<EventHeader>>(
      "test.root",

      "aTree",
      "RecEvent"
      );

  ROOT::RDataFrame rdf(std::move(ds));
  rdf
      .Define("vtx_r", "TMath::Sqrt(vtx_x*vtx_x + vtx_y*vtx_y)")
      .Display("", 5)->Print();

}

TEST(AnalysisTreeRDF, DisplayVtxTracks) {
  makeTestTree(25);

  auto ds = std::make_unique<Impl::AnalysisTreeRDFImplT<TrackDetector>>(
      "test.root",
      "aTree",
      "VtxTracks"
      );

  ROOT::RDataFrame rdf(std::move(ds));
  rdf
  .Display("", 5)->Print();
}

TEST(AnalysisTreeRDF, RealDataExample) {
  makeTestTree(25);

  auto ds = std::make_unique<Impl::AnalysisTreeRDFImplT<TrackDetector>>(
      "test_resources/991.analysistree.root",
      "rTree",
      "VtxTracks"
      );

  ROOT::RDataFrame rdf(std::move(ds));
  rdf
  .Display("", 5)->Print();
}