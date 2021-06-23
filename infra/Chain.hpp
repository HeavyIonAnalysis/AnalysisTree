/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_INFRA_CHAIN_HPP_
#define ANALYSISTREE_INFRA_CHAIN_HPP_

#include <fstream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <TChain.h>
#include <TFile.h>

#include "Configuration.hpp"
#include "DataHeader.hpp"
#include "Utils.hpp"
#include "Branch.hpp"

namespace AnalysisTree {

class Chain : public TChain {

 public:
  Chain() : TChain() {}

  Chain(TTree* tree, Configuration* config, DataHeader* data_header) : TChain(tree->GetName()),
                                                                       configuration_(config),
                                                                       data_header_(data_header) {
    this->AddFriend(tree);
  }

  Chain(const std::string& filename, const std::string& treename) : TChain(treename.c_str()) {
    TFile* file = TFile::Open(filename.c_str(), "read");
    configuration_ = (Configuration*) file->Get("Configuration");
    data_header_ = (DataHeader*) file->Get("DataHeader");
    this->Add(filename.c_str());
  }

  Chain(std::vector<std::string> filelists, std::vector<std::string> treenames) : TChain(treenames.at(0).c_str()),
                                                                                  filelists_(std::move(filelists)),
                                                                                  treenames_(std::move(treenames)) {
    InitChain();
    InitConfiguration();
    InitDataHeader();
  }

  ANALYSISTREE_ATTR_NODISCARD Configuration* GetConfiguration() const { return configuration_; }
  ANALYSISTREE_ATTR_NODISCARD DataHeader* GetDataHeader() const { return data_header_; }
  ANALYSISTREE_ATTR_NODISCARD const std::map<std::string, BranchPointer>& GetBranchPointers() const { return branches_; }
  ANALYSISTREE_ATTR_NODISCARD const std::map<std::string, Matching*>& GetMatchPointers() const { return matches_; }

  void SetDataHeader(DataHeader* dh) { data_header_ = dh; }

  BranchPointer GetPointerToBranch(const std::string& name) const {
    auto br = branches_.find(name);
    if (br != branches_.end()) {
      return br->second;
    } else {
      throw std::runtime_error("Branch " + name + " is not found!");
    }
  }

  /**
 * @brief Loads selected list of branches from TTree
 * @param t
 * @param config
 * @param names List of selected branches. If empty, loads all branches found in Configuration
 */
  void InitPointersToBranches(std::set<std::string> names);

  Long64_t Draw(const char* varexp, const char* selection = nullptr, Option_t* option = "", Long64_t nentries = kMaxEntries, Long64_t firstentry = 0) override;

  void Print(Option_t*) const override {
    this->data_header_->Print();
    this->configuration_->Print();
  }

  class Branch GetBranch(const std::string& name) const {
    auto it = branches_.find(name);
    if(it == branches_.end()){
      throw std::runtime_error("Branch " + name + " is not found!");
    }
    auto ptr = branches_.find(name)->second;
    return AnalysisTree::Branch(configuration_->GetBranchConfig("SimParticles"), ptr);
  }

  template<class BranchT>
  class Branch AddBranch(BranchT* ptr, const BranchConfig& config) {
    configuration_->AddBranchConfig(config);
    ptr = new BranchT(config.GetId());
    this->Branch(config.GetName().c_str(), &ptr);
    return AnalysisTree::Branch(config, ptr);
  }

 protected:
  void InitChain();
  void InitConfiguration();
  void InitDataHeader();

  void DrawTransform(std::string& expr) const;
  static std::vector<std::pair<std::string, int>> FindAndRemoveFields(std::string& expr);
  void DrawFieldTransform(std::string& expr) const;

  static TChain* MakeChain(const std::string& filelist, const std::string& treename);

  /**
 * @brief Loads object of type T from one (first) ROOT file specified in filelist
 * @tparam T type of object
 * @param filelist
 * @param name name of the object
 * @return pointer to the loaded object
 */
  template<class T>
  static T* GetObjectFromFileList(const std::string& filelist, const std::string& name);

  std::string LookupAlias(const std::vector<std::string>& names, const std::string& name, size_t copy = 0);

  std::vector<std::string> filelists_{};
  std::vector<std::string> treenames_{};

  Configuration* configuration_{nullptr};
  DataHeader* data_header_{nullptr};

  std::map<std::string, BranchPointer> branches_{};
  std::map<std::string, Matching*> matches_{};

  ClassDefOverride(AnalysisTree::Chain, 1)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_INFRA_CHAIN_HPP_
