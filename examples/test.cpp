#include <iostream>
#include <random>

#include <AnalysisTree/ToyMC.hpp>
#include <AnalysisTree/AnalysisTask.hpp>

using namespace AnalysisTree;

int main(int argc, char* argv[]) {
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

  auto* var_manager = new AnalysisTask;
  Variable px_sim("SimParticles", "px");
  Variable px_rec("RecTracks", "px");
  Cuts eta_cut("eta_cut", {RangeCut({"SimParticles.eta"}, -1, 1)});
  var_manager->AddEntry(AnalysisEntry({px_sim}));
//  var_manager->AddEntry(AnalysisEntry({px_rec}));
//  var_manager->AddEntry(AnalysisEntry({px_sim, px_rec}));
//var_manager->AddEntry(AnalysisEntry({px_sim, px_rec}, &eta_cut));

  man->AddTask(var_manager);

  man->Init({filelist}, {treename});
  man->Run(-1);
  man->Finish();

}
