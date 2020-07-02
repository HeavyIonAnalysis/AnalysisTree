#ifndef ANALYSISTREE_TEST_CORE_INDEXEDOBJECT_TEST_HPP_
#define ANALYSISTREE_TEST_CORE_INDEXEDOBJECT_TEST_HPP_

TEST(Test_AnalysisTreeCore, Test_WriteIndexedObject) {
  auto *indexedObject = new IndexedObject(1);

  TFile outputFile("Test_WriteIndexedObject.root", "recreate");
  TTree *indexedObjectTree = new TTree("TestIndexedObjectTree", "");
  indexedObjectTree->Branch("indexedObject", &indexedObject);

  indexedObjectTree->Fill();

  indexedObjectTree->Write();
  outputFile.Close();

  IndexedObject *indexedObject1 = nullptr;
  TFile inputFile("Test_WriteIndexedObject.root", "infra");
  indexedObjectTree = (TTree *) inputFile.Get("TestIndexedObjectTree");
  indexedObjectTree->SetBranchAddress("indexedObject", &indexedObject1);
  indexedObjectTree->GetEntry(0L);

  EXPECT_EQ(*indexedObject1, *indexedObject);
}

#endif//ANALYSISTREE_TEST_CORE_INDEXEDOBJECT_TEST_HPP_
