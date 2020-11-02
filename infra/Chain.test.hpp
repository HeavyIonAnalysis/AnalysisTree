#ifndef ANALYSISTREE_INFRA_CHAIN_TEST_HPP_
#define ANALYSISTREE_INFRA_CHAIN_TEST_HPP_

#include <gtest/gtest.h>

#include <infra/Chain.hpp>

namespace{

using namespace AnalysisTree;

TEST(Test_AnalysisTreeInfra, Test_Chain){

  auto* chain = new Chain({"fl_toy_mc.txt"}, {"tTree"});

  EXPECT_EQ(chain->GetNtrees(), 1);
  EXPECT_EQ(chain->GetEntries(), 1000);

  chain->InitPointersToBranches({});
  auto branches_map = chain->GetBranchPointers();

  chain->Draw("SimParticles.px", "");

  auto* particles = std::get<Particles*>(chain->GetPointerToBranch("SimParticles"));
  auto* tracks = std::get<TrackDetector*>(chain->GetPointerToBranch("RecTracks"));

  for(ULong64_t i=0; i<10; ++i){
    chain->GetEntry(i);

    const auto Ms = particles->GetNumberOfChannels();
    const auto Mr = tracks->GetNumberOfChannels();

    EXPECT_GT(Mr, 0);
    EXPECT_GT(Ms, 0);
  }
}

}




#endif//ANALYSISTREE_INFRA_CHAIN_TEST_HPP_
