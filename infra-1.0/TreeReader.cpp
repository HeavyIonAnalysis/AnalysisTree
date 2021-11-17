#include "TreeReader.hpp"
#include <TFileCollection.h>
#include <THashList.h>

namespace AnalysisTree::Version1{

TChain* MakeChain(const std::string& filelist, const std::string& treename) {
  auto chain = new TChain(treename.c_str());
  TFileCollection fc("fc","",filelist.c_str());
  chain->AddFileInfoList(fc.GetList());
  chain->ls();
  return chain;
}

std::string LookupAlias(const std::vector<std::string>& names, const std::string& name, size_t copy) {
  auto full_name = name + "_" + std::to_string(copy);
  auto it = std::find(names.begin(), names.end(), full_name);
  /* not found */
  if (it == names.end()) {
    return full_name;
  }
  return LookupAlias(names, name, copy + 1);
}

TChain* MakeChain(const std::vector<std::string>& filelists, const std::vector<std::string>& treenames) {
  /* TODO remove assert, throw exceptions */
  assert(!filelists.empty() && !treenames.empty() && filelists.size() == treenames.size());
  auto* chain = MakeChain(filelists.at(0), treenames.at(0));

  std::vector<std::string> aliases;
  aliases.reserve(treenames.size());
  for (const auto& tree_name : treenames) {
    aliases.emplace_back(LookupAlias(aliases, tree_name));
  }

  for (uint i = 1; i < filelists.size(); i++) {
    if (aliases.at(i) != treenames.at(i)) {
      std::cout << "Tree '" << treenames.at(i) << "' will be friended under the alias '" << aliases.at(i) << "'" << std::endl;
    }
    chain->AddFriend(MakeChain(filelists.at(i), treenames.at(i)), aliases.at(i).c_str());
  }

  std::cout << "Ntrees = " << chain->GetNtrees() << "\n";
  std::cout << "Nentries = " << chain->GetEntries() << "\n";
  return chain;
}

std::map<std::string, void*> GetPointersToBranches(TChain* t, const AnalysisTree::Configuration& config, std::set<std::string> names) {

  std::cout << "GetPointersToBranches" << std::endl;
  std::map<std::string, void*> ret;

  if (names.empty()) {// all branches by default, if not implicitly specified
    for (const auto& branch : config.GetBranchConfigs()) {
      names.insert(branch.second.GetName());
    }
  }

  for (const auto& branch_name : names) {// Init all pointers to branches
    const auto& branch_config = config.GetBranchConfig(branch_name);
    std::cout << "Adding branch pointer: " << branch_name << std::endl;
    auto emplace_result = ret.emplace(branch_name, nullptr);
    if (!emplace_result.second) {
      throw std::runtime_error("Branch '" + branch_name + "' already has pointer");
    }
    auto &new_element = *emplace_result.first;
    switch (branch_config.GetType()) {
      case DetType::kTrack:
        new_element.second = new TrackDetector;
        t->SetBranchAddress(branch_name.c_str(), (TrackDetector**) &new_element.second);
        break;
      case DetType::kHit:
        new_element.second = new HitDetector;
        t->SetBranchAddress(branch_name.c_str(), (HitDetector**) &new_element.second);
        break;
      case DetType::kEventHeader:
        new_element.second = new EventHeader;
        t->SetBranchAddress(branch_name.c_str(), (EventHeader**) &new_element.second);
        break;
      case DetType::kParticle:
        new_element.second = new Particles;
        t->SetBranchAddress(branch_name.c_str(), (Particles**) &new_element.second);
        break;
      case DetType::kModule:
        new_element.second = new ModuleDetector;
        t->SetBranchAddress(branch_name.c_str(), (ModuleDetector**) &new_element.second);
        break;
    }
  }

  for (const auto& match : config.GetMatches()) {// Init all pointers to matching //TODO exclude unused
    auto match_name = match.second;
    std::cout << "Adding branch pointer: " << match.second << std::endl;
    auto emplace_result = ret.emplace(match.second, nullptr);
    if (!emplace_result.second) {
      throw std::runtime_error("Branch '" + match.second + "' already has pointer");
    }
    auto &new_element = *emplace_result.first;
    new_element.second = new Matching;
    t->SetBranchAddress(match_name.c_str(), (Matching**) &new_element.second);
  }

  t->GetEntry(0);//init pointers
  return ret;
}

}