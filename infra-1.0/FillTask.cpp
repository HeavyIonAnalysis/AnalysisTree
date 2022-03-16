#include "FillTask.hpp"

#include <TChain.h>

namespace AnalysisTree::Version1 {

void FillTask::SetInChain(TChain* in_chain) {
  in_chain_ = in_chain;
}

}// namespace AnalysisTree::Version1