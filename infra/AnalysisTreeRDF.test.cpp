//
// Created by eugene on 17/10/2021.
//

#include "AnalysisTreeRDF.hpp"

#include <iostream>
#include <gtest/gtest.h>

#include <Configuration.hpp>
#include <ROOT/RDataFrame.hxx>

using namespace AnalysisTree;

void makeTestTree(int ne = 5) {

  TFile f("test.root", "recreate");


  BranchConfig rec_event_header("RecEvent", DetType::kEventHeader);
  rec_event_header.AddField<int>("int1", "");

  auto event = new EventHeader;
  event->Init(rec_event_header);

  auto tree = new TTree("aTree", "Test tree");
  tree->Branch("RecEvent", &event);

  for (int i = 0; i < ne; ++i) {
    event->Init(rec_event_header);
    event->SetVertexPosition3(TVector3(i, 10*i, 100*i));
    tree->Fill();
  }

  tree->Write();

  Configuration at_config;
  at_config.AddBranchConfig(rec_event_header);
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
