/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "Chain.hpp"
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

  this->GetEntry(0);
  //  auto* ptr = std::get<Particles*>(this->branches_.find("SimParticles")->second);
  //  std::cout << "SDFSDF " << ptr->GetNumberOfChannels() << std::endl;
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

void Chain::DrawFieldTransform(std::string& expr) const {
  auto dot = expr.find('.');
  auto branch = expr.substr(0, dot);
  auto field = expr.substr(dot + 1);

  const auto& br = configuration_->GetBranchConfig(branch);
  auto type = br.GetFieldType(field);
  auto id = br.GetFieldId(field);
  std::string type_str{};

  switch (type) {
    case (Types::kFloat): {
      type_str = "float";
      break;
    }
    case (Types::kInteger): {
      type_str = "int";
      break;
    }
    case (Types::kBool): {
      type_str = "bool";
      break;
    }
    default: {
      throw std::runtime_error("Field type is not defined");
    }
  }

  expr = Form("%s.channels_.GetField<%s>(%i)", branch.c_str(), type_str.c_str(), id);
}

std::vector<std::pair<std::string, int>> Chain::FindAndRemoveFields(std::string& expr) {

  std::vector<std::pair<std::string, int>> fields{};

  int pos = 0;
  while (expr.find('.', pos) != size_t(-1)) {//TODO fix this
    auto dot = expr.find('.', pos);
    if ((!isdigit(expr[dot - 1]) || !isdigit(expr[dot + 1])) && expr[dot + 1] != ' ') {// is not a number like 1.5 or 1.

      auto begin = dot;
      auto is_allowed_char = [](char c) { return isalpha(c) || isdigit(c) || c == '_'; };
      while (begin > 0 && is_allowed_char(expr[begin - 1])) {
        begin--;
      }
      auto end = dot;
      while (end < expr.size() - 1 && is_allowed_char(expr[end + 1])) {
        end++;
      }
      auto field = expr.substr(begin, end - begin + 1);
      fields.emplace_back(std::make_pair(field, begin));
      expr.erase(begin, end - begin + 1);
      pos = begin;
    } else {
      pos = dot + 1;
    }
  }
  return fields;
}

void Chain::DrawTransform(std::string& expr) const {
  auto fields = FindAndRemoveFields(expr);
  int sum{0};
  for (auto& field : fields) {
    DrawFieldTransform(field.first);
    expr.insert(field.second + sum, field.first);
    sum += field.first.size();
  }
}

Long64_t Chain::Draw(const char* varexp, const char* selection, Option_t* option, Long64_t nentries, Long64_t firstentry) {
  std::string exp{varexp};
  std::string sel{selection ? selection : ""};
  DrawTransform(exp);
  if (!sel.empty()) {
    DrawTransform(sel);
  }
  return TChain::Draw(exp.c_str(), sel.c_str(), option, nentries, firstentry);
}

}// namespace AnalysisTree
