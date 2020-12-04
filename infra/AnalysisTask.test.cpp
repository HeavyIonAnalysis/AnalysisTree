#ifndef ANALYSISTREE_INFRA_ANALYSISTASK_TEST_HPP_
#define ANALYSISTREE_INFRA_ANALYSISTASK_TEST_HPP_

#include <gtest/gtest.h>

#include "AnalysisTask.hpp"
#include "TaskManager.hpp"
#include "ToyMC.hpp"

#include <random>

namespace{

using namespace AnalysisTree;

struct AnalysisTaskTestValue{
  int n_entries_{0};
  double mean_{0.f};
  double sigma_{0.f};
};

typedef std::vector<AnalysisTaskTestValue> AnalysisTaskEntryTest;

class AnalysisTaskTest : public AnalysisTask {

 public:

  AnalysisTaskTest() = default;

  void Init() override {
    AnalysisTask::Init();

    for(const auto& entry : entries_){
      AnalysisTaskEntryTest test_entry(entry.GetVariables().size());
      entries_test_.emplace_back(test_entry);
    }
  }

  void Exec() override {
    AnalysisTask::Exec();

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
    AnalysisTask::Finish();

    for(auto& entry : entries_test_) {
      for(auto& var : entry) {
        var.mean_ /= var.n_entries_;
        var.sigma_ /= var.n_entries_;
      }
    }
  }

  ANALYSISTREE_ATTR_NODISCARD const std::vector<AnalysisTaskEntryTest>& GetEntriesTest() const {
    return entries_test_;
  }

 private:

  std::vector<AnalysisTaskEntryTest> entries_test_{};

};


TEST(AnalysisTask, Basics) {

const int n_events = 1000;  // TODO propagate somehow
std::string filename = "toymc_analysis_task.root";
std::string treename = "tTree";
std::string filelist = "fl_toy_mc.txt";

auto* man = TaskManager::GetInstance();

auto* toy_mc = new ToyMC<std::default_random_engine>;
man->AddTask(toy_mc);
man->SetOutputName(filename, treename);

man->Init();
man->Run(n_events);
man->Finish();

std::ofstream fl(filelist);
fl << filename << "\n";
fl.close();

man = TaskManager::GetInstance();

auto* var_manager = new AnalysisTaskTest;
Variable px_sim("SimParticles", "px");
Variable px_rec("RecTracks", "px");
Cuts eta_cut("eta_cut", {RangeCut({"SimParticles.eta"}, -1, 1)});
var_manager->AddEntry(AnalysisEntry({px_sim}));
var_manager->AddEntry(AnalysisEntry({px_rec}));
var_manager->AddEntry(AnalysisEntry({px_sim, px_rec}));
var_manager->AddEntry(AnalysisEntry({px_sim, px_rec}, &eta_cut));

man->AddTask(var_manager);

man->Init({filelist}, {treename});
man->Run(-1);
var_manager->Finish();

auto px_sim_stat = var_manager->GetEntriesTest().at(0).at(0);
auto px_rec_stat = var_manager->GetEntriesTest().at(1).at(0);

EXPECT_NEAR(px_sim_stat.n_entries_, n_events*100, n_events);
EXPECT_NEAR(px_sim_stat.mean_, 0., 0.05);
EXPECT_NEAR(px_sim_stat.sigma_, 1., 0.1);

EXPECT_NEAR(px_rec_stat.n_entries_, n_events*100, n_events);
EXPECT_NEAR(px_rec_stat.mean_, 0., 0.05);
EXPECT_NEAR(px_rec_stat.sigma_, 1., 0.1);

man->Finish();
}

}
#endif//ANALYSISTREE_INFRA_ANALYSISTASK_TEST_HPP_
