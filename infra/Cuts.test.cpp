#ifndef ANALYSISTREE_INFRA_CUT_TEST_HPP_
#define ANALYSISTREE_INFRA_CUT_TEST_HPP_

#include <gtest/gtest.h>

#include <vector>

#include <TFile.h>
#include <TTree.h>

#include <core/Track.hpp>
#include <core/Configuration.hpp>

#include <infra/Cuts.hpp>
#include <infra/SimpleCut.hpp>

namespace {

using namespace AnalysisTree;

TEST(Cuts, RangeCut) {

  BranchConfig VtxTracksBranch("VtxTracks", DetType::kTrack);
  VtxTracksBranch.AddField<float>("chi2");

  auto* track = new Track();
  track->Init(VtxTracksBranch);
  track->SetMomentum(0.5, 0.4, 4.);
  track->SetField(float(2.4), VtxTracksBranch.GetFieldId("chi2"));

  Configuration conf;
  conf.AddBranchConfig(VtxTracksBranch);

  Variable chi2("VtxTracks", "chi2");

  AnalysisTree::Cuts cut_true("cut1", {{chi2, 2., 3.}});
  cut_true.Init(conf);

  AnalysisTree::Cuts cut_false1("cut1", {{chi2, 0., 2.3999}});
  cut_false1.Init(conf);

  AnalysisTree::Cuts cut_false2("cut1", {{chi2, 2.4001, 3.}});
  cut_false2.Init(conf);

  ASSERT_TRUE(cut_true.Apply(*track));
  ASSERT_FALSE(cut_false1.Apply(*track));
  ASSERT_FALSE(cut_false2.Apply(*track));
}

TEST(Cuts, EqualCut) {

  BranchConfig VtxTracksBranch("VtxTracks", DetType::kTrack);
  VtxTracksBranch.AddField<int>("nhits");
  VtxTracksBranch.AddField<bool>("is_good");
  Configuration conf;
  conf.AddBranchConfig(VtxTracksBranch);

  auto* track = new Track();
  track->Init(VtxTracksBranch);
  track->SetMomentum(0.5, 0.4, 4.);
  track->SetField(3, VtxTracksBranch.GetFieldId("nhits"));
  track->SetField(true, VtxTracksBranch.GetFieldId("is_good"));

  Cuts cut_true1("cut1", {{{"VtxTracks", "nhits"}, 3}});
  cut_true1.Init(conf);

  Cuts cut_true2("cut4", {{{"VtxTracks", "is_good"}, true}});
  cut_true2.Init(conf);

  Cuts cut_false1("cut2", {{{"VtxTracks", "nhits"}, 2}});
  cut_false1.Init(conf);

  Cuts cut_false2("cut3", {{{"VtxTracks", "nhits"}, 4}});
  cut_false2.Init(conf);

  Cuts cut_false3("cut5", {{{"VtxTracks", "is_good"}, false}});
  cut_false3.Init(conf);

  ASSERT_TRUE(cut_true1.Apply(*track));
  ASSERT_TRUE(cut_true2.Apply(*track));

  ASSERT_FALSE(cut_false1.Apply(*track));
  ASSERT_FALSE(cut_false2.Apply(*track));
  ASSERT_FALSE(cut_false3.Apply(*track));
}

TEST(Cuts, FunctionalCut) {

  BranchConfig VtxTracksBranch("VtxTracks", DetType::kTrack);
  VtxTracksBranch.AddFields<float>({"dcax", "dcay", "dcaz"});
  VtxTracksBranch.AddFields<int>({"Nhits_vtpc1", "Nhits_vtpc2", "Nhits_mtpc"});
  Configuration conf;
  conf.AddBranchConfig(VtxTracksBranch);

  auto* track = new Track();
  track->Init(VtxTracksBranch);
  track->SetMomentum(0.5, 0.4, 4.);
  track->SetField(float(0.7), VtxTracksBranch.GetFieldId("dcax"));
  track->SetField(float(1.1), VtxTracksBranch.GetFieldId("dcay"));
  track->SetField(int(15), VtxTracksBranch.GetFieldId("Nhits_vtpc1"));
  track->SetField(int(10), VtxTracksBranch.GetFieldId("Nhits_vtpc2"));
  track->SetField(int(40), VtxTracksBranch.GetFieldId("Nhits_mtpc"));

  SimpleCut testCutDcaXY_true({{"VtxTracks.dcax"}, {"VtxTracks.dcay"}},
                              [](std::vector<double>& dca) { return dca[0] * dca[0] + dca[1] * dca[1] < 2; });
  SimpleCut testCutDcaXY_false({{"VtxTracks.dcax"}, {"VtxTracks.dcay"}},
                               [](std::vector<double>& dca) { return dca[0] * dca[0] + dca[1] * dca[1] < 1; });

  Cuts dca_true("dca_true", {testCutDcaXY_true});
  dca_true.Init(conf);

  Cuts dca_false("dca_false", {testCutDcaXY_false});
  dca_false.Init(conf);

  SimpleCut testCutNhits_true(
      {{"VtxTracks.Nhits_vtpc1"}, {"VtxTracks.Nhits_vtpc2"}, {"VtxTracks.Nhits_mtpc"}},
      [](std::vector<double>& nhits) { return nhits[0] + nhits[1] > 15 && nhits[0] + nhits[1] + nhits[2] > 30; });

  Cuts Nhits_true("Nhits_true", {testCutNhits_true});
  Nhits_true.Init(conf);

  ASSERT_TRUE(dca_true.Apply(*track));
  ASSERT_FALSE(dca_false.Apply(*track));
  ASSERT_TRUE(Nhits_true.Apply(*track));
}

}// namespace

#endif