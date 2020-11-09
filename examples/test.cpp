#include <iostream>

#include <AnalysisTree/AnalysisTask.hpp>
#include <AnalysisTree/TaskManager.hpp>

using namespace AnalysisTree;

int main(int argc, char* argv[]) {

  if (argc < 2) {
    std::cout << "Error! Please use " << std::endl;
    std::cout << " ./example filelist" << std::endl;
    exit(EXIT_FAILURE);
  }

  auto* man = AnalysisTree::TaskManager::GetInstance();

//  auto* var_manager = new AnalysisTask;
//  Variable px_sim("SimParticles", "px");
//  Variable px_rec("RecTracks", "px");
//  Cuts eta_cut("eta_cut", {SimpleCut({"SimParticles", "eta"}, -1, 1)});
//  var_manager->AddEntry(AnalysisEntry({px_sim}));
//  var_manager->AddEntry(AnalysisEntry({px_rec}));
//  var_manager->AddEntry(AnalysisEntry({px_sim, px_rec}));
//  var_manager->AddEntry(AnalysisEntry({px_sim, px_rec}, &eta_cut));
////  var_manager->FillBranchNames();
//
//  man->AddTask(var_manager);
//
//  man->Init({"/home/vklochkov/Soft/AnalysisTree/test/fl.txt"}, {"tTree"});
//  man->Run(3);
//  man->Finish();

  return 0;

}
