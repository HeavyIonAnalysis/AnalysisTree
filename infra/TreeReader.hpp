#ifndef ANALYSISTREE_TREEREADER_H
#define ANALYSISTREE_TREEREADER_H

#include <fstream>
#include <memory>
#include <set>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

#include "BranchReader.hpp"
#include "Configuration.hpp"
#include "Detector.hpp"
#include "EventHeader.hpp"
#include "Matching.hpp"

namespace AnalysisTree {

static inline TChain* MakeChain(const std::string& filelist, const std::string& treename) {
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

static inline TChain* MakeChain(const std::vector<std::string>& filelists, const std::vector<std::string>& treenames) {
  assert(!filelists.empty() && !treenames.empty() && filelists.size() == treenames.size());
  auto* chain = MakeChain(filelists.at(0), treenames.at(0));

  for (uint i = 1; i < filelists.size(); i++) {
    chain->AddFriend(MakeChain(filelists.at(i), treenames.at(i)));
  }

  if (!chain) {
    throw std::runtime_error("AnalysisTree::MakeChain - chain is nullprt");
  }

  std::cout << "Ntrees = " << chain->GetNtrees() << "\n";
  std::cout << "Nentries = " << chain->GetEntries() << "\n";
  return chain;
}

static inline TTree* MakeTree(const std::string& filename, const std::string& treename) {
  auto file = std::unique_ptr<TFile>(TFile::Open(filename.c_str(), "infra"));

  if ((!file) || (file->IsZombie())) {
    throw std::runtime_error("No file or file is zombie. Exit here: AnalysisTree::MakeTree");
  }

  auto* tree = (TTree*) file->Get(treename.c_str());
  return tree;
}

/**
 * @brief Loads object of type T from one (first) ROOT file specified in filelist
 * @tparam T type of object
 * @param filelist
 * @param name name of the object
 * @return pointer to the loaded object
 */
template<class T>
static inline T* GetObjectFromFileList(const std::string& filelist, const std::string& name) {
  std::cout << "GetObjectFromFileList " << filelist << " " << name << std::endl;

  T* object{nullptr};
  std::ifstream in;
  in.open(filelist);
  std::string line;
  in >> line;

  if (!line.empty()) {
    std::cout << line << " " << name << std::endl;
    auto* in_file = TFile::Open(line.data(), "infra");
    object = (T*) in_file->Get(name.c_str());
  }

  if (object == nullptr) {
    throw std::runtime_error("AnalysisTree::GetObjectFromFileList - object is nullprt");
  }

  return object;
}

/**
 * @brief Retrieves Configuration from each of provided filelist, merges different Configurations
 * @param filelists - list of filelists
 * @param name name of Configuration object
 * @return pointer to Configuration object
 */
static inline Configuration* GetConfigurationFromFileList(const std::vector<std::string>& filelists, const std::string& name = "Configuration") {
  assert(!filelists.empty());

  Configuration* config{GetObjectFromFileList<Configuration>(filelists.at(0), name)};

  for (uint i = 1; i < filelists.size(); ++i) {
    auto* config_i = GetObjectFromFileList<Configuration>(filelists.at(i), name);
    for (uint j = 0; j < config_i->GetNumberOfBranches(); ++j) {
      config->AddBranchConfig(config_i->GetBranchConfig(j));
    }
  }
  return config;
}

/**
 * @brief Loads selected list of branches from TTree
 * @param t
 * @param config
 * @param names List of selected branches. If empty, loads all branches found in Configuration
 * @return
 */
static inline std::map<std::string, void*> GetPointersToBranches(TTree* t, const Configuration& config,
                                                                 std::set<std::string> names = {}) {


  std::map<std::string, void*> ret;
  std::map<std::string, BranchPointer> temp;

  if (names.empty()) {// all branches by default, if not implicitly specified
    for (const auto& branch : config.GetBranchConfigs()) {
      names.insert(branch.GetName());
    }
  }

  for (const auto& branch : names) { // Init all pointers to branches
    BranchPointer branch_ptr;
    const auto& branch_config = config.GetBranchConfig(branch);
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
  for (const auto& match : config.GetMatches()) { // Init all pointers to matching //TODO exclude unused
    std::cout << "Adding branch pointer: " << match.second << std::endl;
    matches.emplace(match.second, new Matching);
  }

  for (auto& match : matches) {
    t->SetBranchAddress(match.first.c_str(), &(match.second));
  }
  for (auto& branch_map_entry : temp) {
    std::visit([&t, branch_map_entry](auto&& arg){ t->SetBranchAddress(branch_map_entry.first.c_str(), &(arg)); }, branch_map_entry.second);
  }

  t->GetEntry(0);

  for (const auto& branch_map_entry : temp) {
    void* ptr{nullptr};
    const auto& branch_config = config.GetBranchConfig(branch_map_entry.first);
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
    ret.emplace(branch_map_entry.first, ptr);
  }

  for (const auto& branch_map_entry : matches) {
    void* ptr = branch_map_entry.second;
    ret.emplace(branch_map_entry.first, ptr);
  }
  return ret;
}
}// namespace AnalysisTree
#endif//ANALYSISTREE_TREEREADER_H
