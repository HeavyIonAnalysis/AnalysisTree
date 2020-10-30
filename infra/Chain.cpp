#include "Chain.hpp"

#include "BranchReader.hpp"
#include "Matching.hpp"

#include <TChain.h>
#include <fstream>

namespace AnalysisTree{

void Chain::InitChain(){
  assert(!filelists_.empty() && !treenames_.empty() && filelists_.size() == treenames_.size());

  std::ifstream in;
  in.open(filelists_.at(0));
  std::string line;
  std::cout << "Adding files to chain:" << std::endl;
  while ((in >> line).good()) {
    std::cout << line << std::endl;
    if (!line.empty()) {
      this->AddFile(line.data());
    }
  }

  for (uint i = 1; i < filelists_.size(); i++) {
    this->AddFriend(MakeChain(filelists_.at(i), treenames_.at(i)));
  }

  std::cout << "Ntrees = " << this->GetNtrees() << "\n";
  std::cout << "Nentries = " << this->GetEntries() << "\n";
}

void Chain::InitPointersToBranches(std::set<std::string> names){
  if (names.empty()) {// all branches by default, if not implicitly specified
    for (const auto& branch : configuration_->GetBranchConfigs()) {
      names.insert(branch.GetName());
    }
  }

  for (const auto& branch : names) { // Init all pointers to branches
    BranchPointer branch_ptr;
    const auto& branch_config = configuration_->GetBranchConfig(branch);
    std::cout << "Adding branch pointer: " << branch << std::endl;
    switch (branch_config.GetType()) {
      case DetType::kTrack: {
        branch_ptr = new TrackDetector;
        break;
      }
      case DetType::kHit: {
        branch_ptr = new HitDetector;
        break;
      }
      case DetType::kEventHeader: {
        branch_ptr = new EventHeader;
        break;
      }
      case DetType::kParticle: {
        branch_ptr = new Particles;
        break;
      }
      case DetType::kModule: {
        branch_ptr = new ModuleDetector;
        break;
      }
    }
    branches_.emplace(branch, branch_ptr);
  }

  for (const auto& match : configuration_->GetMatches()) { // Init all pointers to matching //TODO exclude unused
    std::cout << "Adding branch pointer: " << match.second << std::endl;
    matches_.emplace(match.second, new Matching);
  }

  for (auto& match : matches_) {
    this->SetBranchAddress(match.first.c_str(), &(match.second));
  }
  for (auto& branch : branches_) {
    std::visit([this, branch](auto&& arg){ this->SetBranchAddress(branch.first.c_str(), &(arg)); }, branch.second);
  }
}

TChain* Chain::MakeChain(const std::string& filelist, const std::string& treename) {
  auto* chain(new TChain(treename.c_str()));

  std::ifstream in;
  in.open(filelist);
  std::string line;
  std::cout << "Adding files to chain:" << std::endl;
  while ((in >> line).good()) {
    std::cout << line << std::endl;
    if (!line.empty()) {
      chain->AddFile(line.data());
    }
  }

  return chain;
}

void Chain::InitConfiguration(){
  assert(!filelists_.empty());
  std::string name = "Configuration";
  configuration_ = GetObjectFromFileList<Configuration>(filelists_.at(0), name);

  for (uint i = 1; i < filelists_.size(); ++i) {
    auto* config_i = GetObjectFromFileList<Configuration>(filelists_.at(i), name);
    for (uint j = 0; j < config_i->GetNumberOfBranches(); ++j) {
      configuration_->AddBranchConfig(config_i->GetBranchConfig(j));
    }
  }
}

void Chain::InitDataHeader(){
  try{
    data_header_ = GetObjectFromFileList<DataHeader>(filelists_.at(0), "DataHeader");
  }
  catch (const std::runtime_error& err) {
    std::cout << err.what() << std::endl;
  }
}

template<class T>
T* Chain::GetObjectFromFileList(const std::string& filelist, const std::string& name) {
  std::cout << "GetObjectFromFileList " << filelist << " " << name << std::endl;

  T* object{nullptr};
  std::ifstream in;
  in.open(filelist);
  std::string line;
  in >> line;

  if (!line.empty()) {
    std::cout << line << " " << name << std::endl;
    auto* in_file = TFile::Open(line.data(), "infra");
    object = in_file->Get<T>(name.c_str());
  }

  if (object == nullptr) {
    throw std::runtime_error("AnalysisTree::GetObjectFromFileList - object is nullprt");
  }

  return object;
}

}