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
  AnalysisTreeBranch<EventHeader> br(c, nullptr);

  EXPECT_EQ(br.GetFields(), std::vector<std::string>({"vtx_x","vtx_y","vtx_z"}));
  EXPECT_EQ(br.Select("vtx_x")->GetFields(), std::vector<std::string>({"vtx_x"}));

  auto s_result = br.Select("vtx_x");
  s_result->GetFields();
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
  BranchConfig c("test", DetType::kEventHeader);
  auto data = new EventHeader;
  {
    TFile f("tmp.root", "RECREATE");
    auto tree = new TTree("aTree", "");
    tree->Branch(c.GetName().c_str(), &data);

    data->Init(c);
    for (size_t iEv = 0; iEv < 100; ++iEv) {
      data->SetVertexPosition3({float(10 * iEv + 1), float(10 * iEv + 2), float(10 * iEv + 3)});
      tree->Fill();
    }
    tree->Write();
    f.Close();
  }

  TFile f("tmp.root", "READ");
  AnalysisTreeBranch<EventHeader> atb(c, f.Get<TTree>("aTree"));

  for (size_t iEv = 0; iEv < 100; ++iEv) {
    atb.GetEntry(iEv);
    auto results = atb.GetResultsMatrix();
    EXPECT_EQ(results.size(), atb.GetFields().size());
    EXPECT_EQ(results["vtx_x"].size(), atb.GetNumberOfChannels());
    EXPECT_EQ(results["vtx_x"][0], 10*iEv + 1);
    EXPECT_EQ(results["vtx_y"][0], 10*iEv + 2);
    EXPECT_EQ(results["vtx_z"][0], 10*iEv + 3);
  }

}
}

#endif//ANALYSISTREE_INFRA_BRANCHVIEW_TEST_HPP
