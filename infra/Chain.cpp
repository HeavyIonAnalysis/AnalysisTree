/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "Chain.hpp"
#include "ChainDrawHelper.hpp"
#include "BranchReader.hpp"
#include "Matching.hpp"

#include <TChain.h>
#include <TFileCollection.h>

#include <fstream>
#include <iostream>

namespace AnalysisTree {

TChain* Chain::MakeChain(const std::string& filelist, const std::string& treename) {
  auto chain = new TChain(treename.c_str());
  TFileCollection fc("fc", "", filelist.c_str());
  chain->AddFileInfoList(reinterpret_cast<TCollection*>(fc.GetList()));
  chain->ls();
  return chain;
}

std::string Chain::LookupAlias(const std::vector<std::string>& names, const std::string& name, size_t copy) {
  auto full_name = name + "_" + std::to_string(copy);
  auto it = std::find(names.begin(), names.end(), full_name);
  /* not found */
  if (it == names.end()) {
    return full_name;
  }
  return LookupAlias(names, name, copy + 1);
}

void Chain::InitChain() {
  /* TODO remove assert, throw exceptions */
  assert(!filelists_.empty() && !treenames_.empty() && filelists_.size() == treenames_.size());

  TFileCollection fc("fc", "", filelists_[0].c_str());
  this->AddFileInfoList(reinterpret_cast<TCollection*>(fc.GetList()));

  std::vector<std::string> aliases;
  aliases.reserve(treenames_.size());
  for (const auto& tree_name : treenames_) {
    aliases.emplace_back(LookupAlias(aliases, tree_name));
  }

  for (size_t i = 1; i < filelists_.size(); i++) {
    if (aliases.at(i) != treenames_.at(i)) {
      std::cout << "Tree '" << treenames_.at(i) << "' will be friended under the alias '" << aliases.at(i) << "'" << std::endl;
    }
    this->AddFriend(MakeChain(filelists_.at(i), treenames_.at(i)), aliases.at(i).c_str());
  }

  std::cout << "Ntrees = " << this->GetNtrees() << "\n";
  std::cout << "Nentries = " << this->GetEntries() << "\n";
}

void Chain::InitPointersToBranches(std::set<std::string> names) {
  if (names.empty()) {// all branches by default, if not implicitly specified
    for (const auto& branch : configuration_->GetBranchConfigs()) {
      names.insert(branch.second.GetName());
    }
  }

  for (const auto& branch : names) {// Init all pointers to branches
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

  for (const auto& match : configuration_->GetMatches()) {// Init all pointers to matching //TODO exclude unused
    std::cout << "Adding branch pointer: " << match.second << std::endl;
    matches_.emplace(match.second, new Matching);
  }

  for (auto& match : matches_) {
    this->SetBranchAddress(match.first.c_str(), &(match.second));
  }
  for (auto& branch : branches_) {
    ANALYSISTREE_UTILS_VISIT(set_branch_address_struct(this, branch.first), branch.second);
  }
}

void Chain::InitConfiguration() {
  assert(!filelists_.empty());
  std::string name = "Configuration";
  configuration_ = GetObjectFromFileList<Configuration>(filelists_.at(0), name);

  for (size_t i = 1; i < filelists_.size(); ++i) {
    auto* config_i = GetObjectFromFileList<Configuration>(filelists_.at(i), name);
    for (size_t j = 0; j < config_i->GetNumberOfBranches(); ++j) {
      configuration_->AddBranchConfig(config_i->GetBranchConfig(j));
    }
  }
}

void Chain::InitDataHeader() {
  try {
    data_header_ = GetObjectFromFileList<DataHeader>(filelists_.at(0), "DataHeader");
  } catch (const std::runtime_error& err) {
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
    object = (T*) in_file->Get(name.c_str());
  }

  if (object == nullptr) {
    throw std::runtime_error("AnalysisTree::GetObjectFromFileList - object is nullprt");
  }

  return object;
}

Long64_t Chain::Draw(const char* varexp, const char* selection, Option_t* option, Long64_t nentries, Long64_t firstentry) {
  std::string exp{varexp};
  std::string sel{selection ? selection : ""};

  auto helper = ChainDrawHelper(configuration_);
  helper.DrawTransform(exp);
  if (!sel.empty()) {
    helper.DrawTransform(sel);
  }
  return TChain::Draw(exp.c_str(), sel.c_str(), option, nentries, firstentry);
}

}// namespace AnalysisTree
