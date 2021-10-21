/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include <iostream>
#include <Chain.hpp>

using namespace AnalysisTree;

void example(const std::string& filename, const std::string& treename);

int main(int argc, char* argv[]) {

  if (argc < 3) {
    std::cout << "Error! Please use " << std::endl;
    std::cout << " ./example file_name tree_name" << std::endl;
    exit(EXIT_FAILURE);
  }

  const std::string filename = argv[1];
  const std::string treename = argv[2];
  example(filename, treename);

  return 0;
}

void example(const std::string& filename, const std::string& treename){
  auto* chain = new Chain(filename, treename);
  chain->InitPointersToBranches({"VtxTracks", "SimParticles"});

  auto* config = chain->GetConfiguration();
  auto* data_header = chain->GetDataHeader();

  data_header->Print();
  config->Print();

  auto rec_particles = chain->GetBranch("VtxTracks");
  auto rec2sim_particles = chain->GetMatching("VtxTracks", "SimParticles");

  auto rec_pT = rec_particles.GetField("pT");

  for (long i_event = 0; i_event < 10; ++i_event) {
    chain->GetEntry(i_event);
    for(size_t i=0; i<rec_particles.size(); ++i){
      auto pT = rec_particles[i][rec_pT];
      std::cout << " track #" << i << " pT = " << pT << std::endl;
    }
  }
}
