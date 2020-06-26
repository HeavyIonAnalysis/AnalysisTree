#ifdef __MAKECINT__

#pragma link C++ class AnalysisTree::SimpleCut+;
#pragma link C++ class AnalysisTree::Cuts+;
#pragma link C++ class AnalysisTree::Variable+;
#pragma link C++ class AnalysisTree::FillTask+;
#pragma link C++ class AnalysisTree::TaskManager+;
#pragma link C++ class AnalysisTree::BranchReader+;
#pragma link C++ class AnalysisTree::VarManager+;

//#pragma link C++ function AnalysisTree::MakeRDataFrame(TTree &, const AnalysisTree::Configuration &);
#pragma link C++ function AnalysisTree::MakeChain(const std::string& filelist, const std::string& treename);

#pragma link C++ defined_in "TreeReader.h";

#endif
