/* Copyright (C) 2019-2026 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov, Frederic Linz */
#ifndef ANALYSISTREE_CORE_MATCHING_TEST_HPP_
#define ANALYSISTREE_CORE_MATCHING_TEST_HPP_

#include <gtest/gtest.h>

#include "Matching.hpp"

#include <TFile.h>
#include <TTree.h>

#include <stdexcept>
#include <vector>

namespace {

using namespace AnalysisTree;

std::vector<Integer_t> ToVector(const MatchRange& range) {
  return {range.begin(), range.end()};
}

TEST(Matching, Basics) {

  Matching matching(0, 1);

  ASSERT_EQ(matching.GetBranch1Id(), 0);
  ASSERT_EQ(matching.GetBranch2Id(), 1);
  ASSERT_EQ(matching.GetNumberOfPairs(), 0);
  ASSERT_EQ(matching.GetNumberOfIds(), 0);
  ASSERT_EQ(matching.GetMatch(0), UndefValueInt);
  ASSERT_EQ(matching.GetMatch(0, true), UndefValueInt);

  matching.AddMatch(0, 0);
  matching.AddMatch(1, 2);

  ASSERT_EQ(matching.GetNumberOfPairs(), 2);

  ASSERT_EQ(matching.GetMatch(0), 0);
  ASSERT_EQ(matching.GetMatch(1), 2);
  ASSERT_EQ(matching.GetMatch(2), UndefValueInt);
  ASSERT_EQ(matching.GetMatch(-1), UndefValueInt);

  ASSERT_EQ(matching.GetMatch(0, true), 0);
  ASSERT_EQ(matching.GetMatch(2, true), 1);
  ASSERT_EQ(matching.GetMatch(1, true), UndefValueInt);

  EXPECT_THROW(matching.AddMatch(-1, 0), std::invalid_argument);
  EXPECT_THROW(matching.AddMatch(0, -1), std::invalid_argument);
}

TEST(Matching, MultiplePartners) {
  // track 0 -> 5, track 1 -> 5 (n:1), track 2 -> {7, 8} (1:n), track 3 without partner, track 4 -> 1
  Matching matching(0, 1);
  matching.AddMatch(0, 5);
  matching.AddMatch(1, 5);
  matching.AddMatch(2, 7);
  matching.AddMatch(2, 8);
  matching.AddMatch(4, 1);

  EXPECT_EQ(matching.GetNumberOfPairs(), 5);
  EXPECT_EQ(matching.GetNumberOfIds(), 5);
  EXPECT_EQ(ToVector(matching.GetMatchesDirect(2)), (std::vector<Integer_t>{7, 8}));
  EXPECT_TRUE(matching.GetMatchesDirect(3).empty());
  EXPECT_TRUE(matching.GetMatchesDirect(100).empty());
  EXPECT_EQ(matching.GetMatch(2), 7);// first added = best partner

  EXPECT_EQ(matching.GetNumberOfIds(true), 9);
  EXPECT_EQ(ToVector(matching.GetMatchesInverted(5)), (std::vector<Integer_t>{0, 1}));
  EXPECT_EQ(ToVector(matching.GetMatches(8, true)), (std::vector<Integer_t>{2}));
  EXPECT_TRUE(matching.GetMatchesInverted(6).empty());
  EXPECT_EQ(matching.GetMatch(5, true), 0);

  matching.Clear();
  EXPECT_EQ(matching.GetNumberOfPairs(), 0);
  EXPECT_EQ(matching.GetNumberOfIds(), 0);
  EXPECT_EQ(matching.GetNumberOfIds(true), 0);
  EXPECT_EQ(matching.GetMatch(2), UndefValueInt);
  EXPECT_EQ(matching.GetMatch(5, true), UndefValueInt);
}

TEST(Matching, UnorderedInsertion) {
  // e.g. a converter looping over hits instead of tracks
  Matching matching(0, 1);
  matching.AddMatch(3, 30);
  matching.AddMatch(1, 10);
  matching.AddMatch(3, 31);
  matching.AddMatch(0, 0);
  matching.AddMatch(1, 11);

  EXPECT_EQ(ToVector(matching.GetMatchesDirect(0)), (std::vector<Integer_t>{0}));
  EXPECT_EQ(ToVector(matching.GetMatchesDirect(1)), (std::vector<Integer_t>{10, 11}));
  EXPECT_TRUE(matching.GetMatchesDirect(2).empty());
  EXPECT_EQ(ToVector(matching.GetMatchesDirect(3)), (std::vector<Integer_t>{30, 31}));
}

TEST(Matching, InverseFollowsModifications) {
  Matching matching(0, 1);
  matching.AddMatch(0, 5);
  EXPECT_EQ(matching.GetMatch(5, true), 0);

  matching.AddMatch(1, 6);// must invalidate the cached inverse index
  EXPECT_EQ(matching.GetMatch(6, true), 1);
}

// gives access to the legacy maps, as filled by ROOT when reading a file of version <= 2
class LegacyMatching : public Matching {
 public:
  void SetLegacyMaps(std::map<Integer_t, Integer_t> match, std::map<Integer_t, Integer_t> match_inverted) {
    match_ = std::move(match);
    match_inverted_ = std::move(match_inverted);
  }
};

TEST(Matching, LegacyMaps) {
  // v2 content of track 0 -> 5, track 1 -> 5, track 2 -> {7, 8}: each map lost one pair
  LegacyMatching matching;
  matching.SetLegacyMaps({{0, 5}, {1, 5}, {2, 7}}, {{5, 0}, {7, 2}, {8, 2}});

  EXPECT_EQ(matching.GetNumberOfPairs(), 4);
  EXPECT_EQ(matching.GetMatch(0), 5);
  EXPECT_EQ(ToVector(matching.GetMatchesDirect(2)), (std::vector<Integer_t>{7, 8}));// direct pair first
  EXPECT_EQ(ToVector(matching.GetMatchesInverted(5)), (std::vector<Integer_t>{0, 1}));

  // next "entry": the CSR content is replaced, nothing is left over
  matching.SetLegacyMaps({{3, 1}}, {{1, 3}});
  EXPECT_EQ(matching.GetNumberOfPairs(), 1);
  EXPECT_EQ(matching.GetMatch(3), 1);
  EXPECT_EQ(matching.GetMatch(0), UndefValueInt);
  EXPECT_EQ(matching.GetMatch(1, true), 3);
}

TEST(Matching, WriteRead) {
  const std::string file_name = "Test_WriteMatching.root";
  const std::vector<std::vector<std::pair<Integer_t, Integer_t>>> entries{
      {{0, 5}, {1, 5}, {2, 7}, {2, 8}},// 1:1, n:1 and 1:n
      {{3, 1}},                        // different content: nothing may be left over from entry 0
      {}};                             // empty

  {
    auto* matching = new Matching(1, 2);
    TFile output_file(file_name.c_str(), "recreate");
    auto* tree = new TTree("TestMatchingTree", "");
    tree->Branch("Branch1ToBranch2.", &matching);// split, as in TaskManager::AddMatching()
    for (const auto& pairs : entries) {
      matching->Clear();
      for (const auto& p : pairs) {
        matching->AddMatch(p.first, p.second);
      }
      tree->Fill();
    }
    tree->Write();
    output_file.Close();
    delete matching;
  }

  Matching* matching = nullptr;
  TFile input_file(file_name.c_str(), "read");
  auto* tree = (TTree*) input_file.Get("TestMatchingTree");
  ASSERT_NE(tree, nullptr);
  tree->SetBranchAddress("Branch1ToBranch2.", &matching);

  for (size_t i_entry = 0; i_entry < entries.size(); ++i_entry) {
    tree->GetEntry(i_entry);
    ASSERT_NE(matching, nullptr);
    EXPECT_EQ(matching->GetBranch1Id(), 1);
    EXPECT_EQ(matching->GetBranch2Id(), 2);
    ASSERT_EQ(matching->GetNumberOfPairs(), entries[i_entry].size()) << "entry " << i_entry;

    Matching expected(1, 2);
    for (const auto& p : entries[i_entry]) {
      expected.AddMatch(p.first, p.second);
    }
    for (Integer_t id = 0; id < 10; ++id) {
      EXPECT_EQ(ToVector(matching->GetMatchesDirect(id)), ToVector(expected.GetMatchesDirect(id)))
          << "entry " << i_entry << ", id1 " << id;
      // the inverse index is cached: it must be rebuilt for every entry
      EXPECT_EQ(ToVector(matching->GetMatchesInverted(id)), ToVector(expected.GetMatchesInverted(id)))
          << "entry " << i_entry << ", id2 " << id;
    }
  }
}

}// namespace

#endif//ANALYSISTREE_CORE_MATCHING_TEST_HPP_
