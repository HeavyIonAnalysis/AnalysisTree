#ifndef ANALYSISTREE_CORE_TOYMC_TEST_HPP_
#define ANALYSISTREE_CORE_TOYMC_TEST_HPP_

#include <gtest/gtest.h>

#include "TTree.h"
#include "Configuration.hpp"
#include "DataHeader.hpp"

#include "Detector.test.hpp"

namespace AnalysisTree{

class ToyMC {
 public:
  ToyMC() = default;
  ~ToyMC() = default;

  void GenerateEvents(long n_events){
    out_tree_ = new TTree("tTree", "test tree");

    auto* particles = new TestParticles<std::default_random_engine>;
    particles->Init();
    Particles* temp = particles;
//    TestTracks* tracks = new TestTracks;
    out_tree_->Branch("McParticles", &temp);

    for (long i_event = 0; i_event<n_events; ++i_event) {
      particles->FillDetector(10);
      out_tree_->Fill();
    }
  }

  void WriteToFile(const std::string& filename){
    TFile* file{TFile::Open(filename.c_str(), "recreate")};
    out_tree_->Write();
    config_.Write("Configuration");
    dh_.Write("DataHeader");
    file->Close();
  }

 protected:

  Configuration config_;
  DataHeader dh_;
  TTree* out_tree_{nullptr};

};

TEST(Test_AnalysisTreeCore, Test_ToyMC) {
  ToyMC toy_mc;
  toy_mc.GenerateEvents(1000);
  toy_mc.WriteToFile("toy_mc.root");
}

}

#endif //ANALYSISTREE_CORE_TOYMC_TEST_HPP_
