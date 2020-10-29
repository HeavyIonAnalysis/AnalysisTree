#include "Chain.hpp"

#include "BranchReader.hpp"
#include "Matching.hpp"


#include <TChain.h>
#include <fstream>

namespace AnalysisTree{

void Chain::InitChain(const std::vector<std::string>& filelists, const std::vector<std::string>& treenames){
  assert(!filelists.empty() && !treenames.empty() && filelists.size() == treenames.size());
  chain_ = MakeChain(filelists.at(0), treenames.at(0));

  for (uint i = 1; i < filelists.size(); i++) {
    chain_->AddFriend(MakeChain(filelists.at(i), treenames.at(i)));
  }

  if (!chain_) {
    throw std::runtime_error("AnalysisTree::MakeChain - chain is nullprt");
  }
  std::cout << "Ntrees = " << chain_->GetNtrees() << "\n";
  std::cout << "Nentries = " << chain_->GetEntries() << "\n";
}

void Chain::InitPointersToBranches(std::set<std::string> names){

  std::map<std::string, BranchPointer> temp;

  if (names.empty()) {// all branches by default, if not implicitly specified
    for (const auto& branch : configuration_.GetBranchConfigs()) {
      names.insert(branch.GetName());
    }
  }

  for (const auto& branch : names) { // Init all pointers to branches
    BranchPointer branch_ptr;
    const auto& branch_config = configuration_.GetBranchConfig(branch);
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
    temp.emplace(branch, branch_ptr);
  }

  std::map<std::string, Matching*> matches;
  for (const auto& match : configuration_.GetMatches()) { // Init all pointers to matching //TODO exclude unused
    std::cout << "Adding branch pointer: " << match.second << std::endl;
    matches.emplace(match.second, new Matching);
  }

  for (auto& match : matches) {
    chain_->SetBranchAddress(match.first.c_str(), &(match.second));
  }
  for (auto& branch_map_entry : temp) {
    std::visit([this, branch_map_entry](auto&& arg){ chain_->SetBranchAddress(branch_map_entry.first.c_str(), &(arg)); }, branch_map_entry.second);
  }

  chain_->GetEntry(0);

  for (const auto& branch_map_entry : temp) {
    void* ptr{nullptr};
    const auto& branch_config = configuration_.GetBranchConfig(branch_map_entry.first);
    switch (branch_config.GetType()) {
      case DetType::kTrack: {
        ptr = std::get<TrackDetector*>(branch_map_entry.second);
        break;
      }
      case DetType::kHit: {
        ptr = std::get<HitDetector*>(branch_map_entry.second);
        break;
      }
      case DetType::kEventHeader: {
        ptr = std::get<EventHeader*>(branch_map_entry.second);
        break;
      }
      case DetType::kParticle: {
        ptr = std::get<Particles *>(branch_map_entry.second);
        break;
      }
      case DetType::kModule: {
        ptr = std::get<ModuleDetector *>(branch_map_entry.second);
        break;
      }
    }
    branches_.emplace(branch_map_entry.first, ptr);
  }

  for (const auto& branch_map_entry : matches) {
    void* ptr = branch_map_entry.second;
    branches_.emplace(branch_map_entry.first, ptr);
  };

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
  configuration_ = *GetObjectFromFileList<Configuration>(filelists_.at(0), name);

  for (uint i = 1; i < filelists_.size(); ++i) {
    auto* config_i = GetObjectFromFileList<Configuration>(filelists_.at(i), name);
    for (uint j = 0; j < config_i->GetNumberOfBranches(); ++j) {
      configuration_.AddBranchConfig(config_i->GetBranchConfig(j));
    }
  }
}

void Chain::InitDataHeader(){
  try{
    data_header_ = *GetObjectFromFileList<DataHeader>(filelists_.at(0), "DataHeader");
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