/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include <iostream>

#include <AnalysisTree/DataHeader.hpp>
#include <AnalysisTree/EventHeader.hpp>
#include <AnalysisTree/Matching.hpp>

#include <AnalysisTree/Chain.hpp>

using namespace AnalysisTree;

int main(int argc, char* argv[]) {

  if (argc < 2) {
    std::cout << "Error! Please use " << std::endl;
    std::cout << " ./example filelist_ana" << std::endl;
    exit(EXIT_FAILURE);
  }

  const std::string filelist_ana = argv[1];

  auto* chain = new Chain({filelist_ana}, {"aTree"});
  
  auto* config = chain->GetConfiguration();
  auto* data_header = chain->GetDataHeader();

  data_header->Print();
  config->Print();

  auto rec_particles = chain->GetBranch("RecParticles");
  auto rec2sim_particles = chain->GetMatching("RecParticles", "SimParticles");

  auto rec_pT = rec_particles.GetField("pT");

  for (long i_event = 0; i_event < 10; ++i_event) {
    chain->GetEntry(i_event);
    for(int i=0; i<rec_particles.size(); ++i){
      auto pT = rec_particles[i][rec_pT];
    }
  }
}
