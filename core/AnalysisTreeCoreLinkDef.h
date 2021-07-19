#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;

#pragma link C++ namespace AnalysisTree;

#pragma link C++ class AnalysisTree::IndexedObject+;
#pragma link C++ class AnalysisTree::IndexAccessor+;
#pragma link C++ class AnalysisTree::Hit+;
#pragma link C++ class AnalysisTree::Track+;
#pragma link C++ class AnalysisTree::Particle+;
#pragma link C++ class AnalysisTree::Module+;
#pragma link C++ class AnalysisTree::ModulePosition+;
#pragma link C++ class AnalysisTree::EventHeader+;
#pragma link C++ class AnalysisTree::Matching+;

#pragma link C++ class AnalysisTree::VectorConfig<int>+;
#pragma link C++ class AnalysisTree::VectorConfig<float>+;
#pragma link C++ class AnalysisTree::VectorConfig<bool>+;
#pragma link C++ class AnalysisTree::ConfigElement+;
#pragma link C++ class AnalysisTree::Configuration_v3+;
#pragma link C++ class AnalysisTree::Configuration-;
#pragma link C++ class AnalysisTree::BranchConfig+;
#pragma link C++ class AnalysisTree::MatchingConfig+;
#pragma link C++ class AnalysisTree::DataHeader+;

#pragma link C++ class AnalysisTree::Container+;

#pragma link C++ class AnalysisTree::Detector<AnalysisTree::Hit>+;
#pragma link C++ class AnalysisTree::Detector<AnalysisTree::Track>+;
#pragma link C++ class AnalysisTree::Detector<AnalysisTree::Module>+;
#pragma link C++ class AnalysisTree::Detector<AnalysisTree::ModulePosition>+;
#pragma link C++ class AnalysisTree::Detector<AnalysisTree::Particle>+;
#pragma link C++ class AnalysisTree::Detector<AnalysisTree::Container>+;

#pragma link C++ typedef AnalysisTree::TrackDetector;
#pragma link C++ typedef AnalysisTree::Particles;
#pragma link C++ typedef AnalysisTree::ModuleDetector;
#pragma link C++ typedef AnalysisTree::HitDetector;
#pragma link C++ typedef AnalysisTree::ModulePositions;

#pragma link C++ defined_in "Constants.h";

#pragma read sourceClass="AnalysisTree::Configuration" targetClass="AnalysisTree::Configuration_v3";

#endif
