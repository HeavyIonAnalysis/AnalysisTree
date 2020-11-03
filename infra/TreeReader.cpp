#include "TreeReader.hpp"
#include "TFileCollection.h"

TChain* AnalysisTree::MakeChain(const std::string& filelist, const std::string& treename) {
  auto chain = new TChain(treename.c_str());
  TFileCollection fc("fc","",filelist.c_str());
  chain->AddFileInfoList(reinterpret_cast<TCollection*>(fc.GetList()));
  chain->ls();
  return chain;
}

std::string AnalysisTree::LookupAlias(const std::vector<std::string>& names, const std::string& name, size_t copy) {
  auto full_name = name + "_" + std::to_string(copy);
  auto it = std::find(names.begin(), names.end(), full_name);
  /* not found */
  if (it == names.end()) {
    return full_name;
  }
  return LookupAlias(names, name, copy + 1);
}

TChain* AnalysisTree::MakeChain(const std::vector<std::string>& filelists, const std::vector<std::string>& treenames) {
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
