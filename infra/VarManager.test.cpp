#ifndef ANALYSISTREE_INFRA_VARMANAGER_TEST_HPP_
#define ANALYSISTREE_INFRA_VARMANAGER_TEST_HPP_

#include <gtest/gtest.h>

#include "VarManager.hpp"
#include "TaskManager.hpp"

namespace{

using namespace AnalysisTree;

struct VarManagerTestValue{
  int n_entries_{0};
  double mean_{0.f};
  double sigma_{0.f};
};

typedef std::vector<VarManagerTestValue> VarManagerEntryTest;

class VarManagerTest : public VarManager{

 public:

  VarManagerTest() = default;

  void Init(std::map<std::string, void*>& pointers_map) override {
    VarManager::Init(pointers_map);

    for(const auto& entry : entries_){
      VarManagerEntryTest test_entry(entry.GetVariables().size());
      entries_test_.emplace_back(test_entry);
    }
  }

  void Exec() override {
    VarManager::Exec();

    for(uint i=0; i<entries_.size(); ++i) {
      const auto& channels = entries_.at(i).GetValues();
      auto& test_vars = entries_test_.at(i);

      for(const auto& channel : channels){
        for(uint j = 0; j < channel.size(); ++j){  // vars
          auto& tvar = test_vars.at(j);
          auto var = channel.at(j);

          tvar.n_entries_++;
          tvar.mean_ += var;
          tvar.sigma_ += var*var;
        }
      }
    }
  }

  void Finish() override {
    VarManager::Finish();

    for(auto& entry : entries_test_) {
      for(auto& var : entry) {
        var.mean_ /= var.n_entries_;
        var.sigma_ /= var.n_entries_;
      }
    }
  }

  const std::vector<VarManagerEntryTest>& GetEntriesTest() const {
    return entries_test_;
  }

 private:

  std::vector<VarManagerEntryTest> entries_test_{};

};


TEST(VarManager, Basics) {

  const int n_events = 1000;  // TODO propagate somehow
  TaskManager man({"fl_toy_mc.txt"}, {"tTree"});

  auto* var_manager = new VarManagerTest;
  Variable px_sim("SimParticles", "px");
  Variable px_rec("RecTracks", "px");
  Cuts eta_cut("eta_cut", {SimpleCut({"SimParticles", "eta"}, -1, 1)});
  var_manager->AddEntry(VarManagerEntry({px_sim}));
  var_manager->AddEntry(VarManagerEntry({px_rec}));
  var_manager->AddEntry(VarManagerEntry({px_sim, px_rec}));
  var_manager->AddEntry(VarManagerEntry({px_sim, px_rec}, &eta_cut));
  var_manager->FillBranchNames();

  man.AddTask(var_manager);

  man.Init();
  man.Run(-1);
  man.Finish();

  auto px_sim_stat = var_manager->GetEntriesTest().at(0).at(0);
  auto px_rec_stat = var_manager->GetEntriesTest().at(1).at(0);

  EXPECT_NEAR(px_sim_stat.n_entries_, n_events*100, n_events);
  EXPECT_NEAR(px_sim_stat.mean_, 0., 0.05);
  EXPECT_NEAR(px_sim_stat.sigma_, 1., 0.1);

  EXPECT_NEAR(px_rec_stat.n_entries_, n_events*100, n_events);
  EXPECT_NEAR(px_rec_stat.mean_, 0., 0.05);
  EXPECT_NEAR(px_rec_stat.sigma_, 1., 0.1);


}

}



#endif //ANALYSISTREE_INFRA_VARMANAGER_TEST_HPP_
