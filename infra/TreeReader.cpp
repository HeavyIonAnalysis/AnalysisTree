#include "TreeReader.hpp"
#include "TFileCollection.h"

TChain* AnalysisTree::MakeChain(const std::string& filelist, const std::string& treename) {
  auto chain = new TChain(treename.c_str());
  TFileCollection fc("fc","",filelist.c_str());
  chain->AddFileInfoList(reinterpret_cast<TCollection*>(fc.GetList()));
  chain->ls();
  return chain;
}
