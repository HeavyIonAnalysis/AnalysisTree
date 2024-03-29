#include "TreeReader.hpp"
#include <TFileCollection.h>
#include <THashList.h>

namespace AnalysisTree::Version1 {

TChain* MakeChain(const std::string& filelist, const std::string& treename) {
  auto chain = new TChain(treename.c_str());
  TFileCollection fc("fc", "", filelist.c_str());
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
    auto& new_element = *emplace_result.first;
    switch (branch_config.GetType()) {
      case DetType::kTrack:
        new_element.second = new TrackDetector;
        if (CheckBranchExistence(t, branch_name) == 1)
          t->SetBranchAddress(branch_name.c_str(), (TrackDetector**) &new_element.second);
        else if (CheckBranchExistence(t, branch_name) == 2)
          t->SetBranchAddress((branch_name + ".").c_str(), (TrackDetector**) &new_element.second);
        else
          throw std::runtime_error("AnalysisTree::TreeReader::GetPointersToBranches - Branch " + branch_name + " does not exist");
        break;
      case DetType::kHit:
        new_element.second = new HitDetector;
        if (CheckBranchExistence(t, branch_name) == 1)
          t->SetBranchAddress(branch_name.c_str(), (HitDetector**) &new_element.second);
        else if (CheckBranchExistence(t, branch_name) == 2)
          t->SetBranchAddress((branch_name + ".").c_str(), (HitDetector**) &new_element.second);
        else
          throw std::runtime_error("AnalysisTree::TreeReader::GetPointersToBranches - Branch " + branch_name + " does not exist");
        break;
      case DetType::kEventHeader:
        new_element.second = new EventHeader;
        if (CheckBranchExistence(t, branch_name) == 1)
          t->SetBranchAddress(branch_name.c_str(), (EventHeader**) &new_element.second);
        else if (CheckBranchExistence(t, branch_name) == 2)
          t->SetBranchAddress((branch_name + ".").c_str(), (EventHeader**) &new_element.second);
        else
          throw std::runtime_error("AnalysisTree::TreeReader::GetPointersToBranches - Branch " + branch_name + " does not exist");
        break;
      case DetType::kParticle:
        new_element.second = new Particles;
        if (CheckBranchExistence(t, branch_name) == 1)
          t->SetBranchAddress(branch_name.c_str(), (Particles**) &new_element.second);
        else if (CheckBranchExistence(t, branch_name) == 2)
          t->SetBranchAddress((branch_name + ".").c_str(), (Particles**) &new_element.second);
        else
          throw std::runtime_error("AnalysisTree::TreeReader::GetPointersToBranches - Branch " + branch_name + " does not exist");
        break;
      case DetType::kModule:
        new_element.second = new ModuleDetector;
        if (CheckBranchExistence(t, branch_name) == 1)
          t->SetBranchAddress(branch_name.c_str(), (ModuleDetector**) &new_element.second);
        else if (CheckBranchExistence(t, branch_name) == 2)
          t->SetBranchAddress((branch_name + ".").c_str(), (ModuleDetector**) &new_element.second);
        else
          throw std::runtime_error("AnalysisTree::TreeReader::GetPointersToBranches - Branch " + branch_name + " does not exist");
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
    auto& new_element = *emplace_result.first;
    new_element.second = new Matching;
    if (CheckBranchExistence(t, match_name) == 1)
      t->SetBranchAddress(match_name.c_str(), (Matching**) &new_element.second);
    else if (CheckBranchExistence(t, match_name) == 2)
      t->SetBranchAddress((match_name + ".").c_str(), (Matching**) &new_element.second);
    else
      throw std::runtime_error("AnalysisTree::TreeReader::GetPointersToBranches - Matching " + match_name + " does not exist");
  }

  t->GetEntry(0);//init pointers
  return ret;
}

int CheckBranchExistence(TChain* chain, const std::string& branchname) {
  std::vector<TChain*> v_chains;
  v_chains.push_back(chain);

  auto* lof = chain->GetListOfFriends();
  if (lof != nullptr) {
    const int Nfriends = lof->GetSize();
    for (int i = 0; i < Nfriends; i++) {
      std::string friend_name = lof->At(i)->GetName();
      TTree* fr = chain->GetFriend(friend_name.c_str());
      v_chains.emplace_back((TChain*) fr);
    }
  }

  for (auto& ch : v_chains) {
    auto* lob = ch->GetListOfBranches();
    const int Nbranches = lob->GetEntries();
    for (int i = 0; i < Nbranches; i++) {
      const std::string& name_i = lob->At(i)->GetName();
      if (name_i == branchname) {
        return 1;
      } else if (name_i == branchname + ".") {
        return 2;
      }
    }
  }

  throw std::runtime_error("AnalysisTree::TreeReader - Branch " + branchname + " does not exist");
  return 0;
}

}// namespace AnalysisTree::Version1