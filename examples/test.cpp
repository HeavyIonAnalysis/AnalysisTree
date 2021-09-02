/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include <iostream>

#include <AnalysisTree/Branch.hpp>
#include <AnalysisTree/ToyMC.hpp>

#include "UserTaskRead.hpp"

#include "TLeaf.h"

using namespace AnalysisTree;

void remove_branches(TTree* tree, AnalysisTree::Configuration* conf){
//    tree->SetBranchStatus("*", false);
//    for(const auto& br : conf->GetBranchConfigs()){
//      auto name = br.second.GetName();
//      std::cout << br.second.GetName() << "  " << (name.find("VtxTracks") == std::string::npos) << std::endl;
//      if(name.find("VtxTracks") == std::string::npos){
//        tree->SetBranchStatus(name.c_str(), true);
//      }
//    }

  auto branch = tree->GetBranch("VtxTracks");
  tree->GetListOfBranches()->Remove(branch);

  TObject* l= tree->GetLeaf("VtxTracks");
  tree->GetListOfLeaves()->Remove(l);

//  for(auto leaf : *branch->GetListOfLeaves()){
//    std::cout << leaf->GetName() << std::endl;
//    tree->GetListOfLeaves()->Remove(leaf);
//  }
}

int main() {

  auto f = TFile::Open("1.analysistree.root", "read");
  auto tree = f->Get<TTree>("rTree");
  auto conf = f->Get<AnalysisTree::Configuration>("Configuration");

  TFile newfile("pid.analysistree.root", "recreate");
  auto new_tree = tree->CloneTree();
  remove_branches(new_tree, conf);
  new_tree->SetName("rrrr");
  new_tree->Write();
  newfile.Close();

  AnalysisTree::TrackDetector* vtx_tracks{nullptr};
  tree->SetBranchAddress("VtxTracks", &vtx_tracks);
  tree->GetEntry(1);
  std::cout << vtx_tracks->GetNumberOfChannels() << std::endl;

  f->Close();
}
