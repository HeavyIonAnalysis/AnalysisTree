#include "FillTask.hpp"

#include <TChain.h>
#include <TTree.h>

#include "DataHeader.hpp"
#include "Configuration.hpp"

void AnalysisTree::FillTask::SetInChain(TChain* in_chain) {
  in_chain_ = in_chain;
}
