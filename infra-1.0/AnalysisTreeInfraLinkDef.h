#ifdef __MAKECINT__

#pragma link C++ namespace AnalysisTree::Version1+;

#pragma link C++ class AnalysisTree::Version1::SimpleCut+;
#pragma link C++ function AnalysisTree::Version1::RangeCut;
#pragma link C++ function AnalysisTree::Version1::EqualsCut;
#pragma link C++ class AnalysisTree::Version1::Cuts+;
#pragma link C++ class AnalysisTree::Version1::Variable+;
#pragma link C++ class AnalysisTree::Version1::FillTask+;
#pragma link C++ class AnalysisTree::Version1::TaskManager+;

#pragma link C++ class AnalysisTree::Version1::BranchReader+;
#pragma link C++ class AnalysisTree::Version1::VarManager+;
#pragma link C++ class AnalysisTree::Version1::VarManagerEntry+;

//#pragma link C++ function AnalysisTree::Version1::MakeRDataFrame(TTree &, const AnalysisTree::Version1::Configuration &);
#pragma link C++ function AnalysisTree::Version1::MakeChain(const std::string& filelist, const std::string& treename);

#pragma link C++ defined_in "TreeReader.h";

#endif
