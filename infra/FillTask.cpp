#include "FillTask.hpp"

#include <TChain.h>

void AnalysisTree::FillTask::SetInChain(TChain* in_chain) {
  in_chain_ = in_chain;
}
