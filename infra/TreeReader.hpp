#ifndef ANALYSISTREE_TREEREADER_H
#define ANALYSISTREE_TREEREADER_H

#include <fstream>
#include <memory>
#include <set>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

#include "Configuration.hpp"
#include "Detector.hpp"
#include "EventHeader.hpp"
#include "Matching.hpp"

namespace AnalysisTree {

TChain* MakeChain(const std::string& filelist, const std::string& treename);

std::string
LookupAlias(const std::vector<std::string>& names, const std::string& name, size_t copy = 0);

TChain* MakeChain(const std::vector<std::string>& filelists, const std::vector<std::string>& treenames);

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
std::map<std::string, void*>
GetPointersToBranches(TChain* t, const Configuration& config,
                      std::set<std::string> names = {});

}// namespace AnalysisTree
#endif//ANALYSISTREE_TREEREADER_H
