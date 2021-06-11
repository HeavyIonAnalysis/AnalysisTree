/* Copyright (C) 2019-2021 GSI, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Eugeny Kashirin, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_TEST_CORE_INDEXEDOBJECT_TEST_HPP_
#define ANALYSISTREE_TEST_CORE_INDEXEDOBJECT_TEST_HPP_

#include <gtest/gtest.h>

#include "IndexedObject.hpp"
#include <TFile.h>
#include <TTree.h>

namespace {

using namespace AnalysisTree;

TEST(IndexedObject, Write) {
  auto* indexedObject = new IndexedObject(1);

  TFile outputFile("Test_WriteIndexedObject.root", "recreate");
  auto* indexedObjectTree = new TTree("TestIndexedObjectTree", "");
  indexedObjectTree->Branch("indexedObject", &indexedObject);

  indexedObjectTree->Fill();

  indexedObjectTree->Write();
  outputFile.Close();

  IndexedObject* indexedObject1 = nullptr;
  TFile inputFile("Test_WriteIndexedObject.root", "infra");
  indexedObjectTree = (TTree*) inputFile.Get("TestIndexedObjectTree");
  indexedObjectTree->SetBranchAddress("indexedObject", &indexedObject1);
  indexedObjectTree->GetEntry(0L);

  EXPECT_EQ(*indexedObject1, *indexedObject);
}
}// namespace
#endif//ANALYSISTREE_TEST_CORE_INDEXEDOBJECT_TEST_HPP_
