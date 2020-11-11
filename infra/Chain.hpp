#ifndef ANALYSISTREE_INFRA_CHAIN_HPP_
#define ANALYSISTREE_INFRA_CHAIN_HPP_

#include <fstream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <TFile.h>
#include <TChain.h>

#include "BranchReader.hpp"
#include "Configuration.hpp"
#include "DataHeader.hpp"

namespace AnalysisTree{

class Chain : public TChain {

 public:

  Chain() : TChain() {};

  Chain(const std::string& name) :
     TChain(name.c_str()),
     configuration_(new Configuration),
     data_header_(new DataHeader)
  {};

  Chain(std::vector<std::string> filelists, std::vector<std::string> treenames) :
   TChain(treenames.at(0).c_str()),
   filelists_(std::move(filelists)),
   treenames_(std::move(treenames))
  {
    InitChain();
    InitConfiguration();
    InitDataHeader();
  };

  [[nodiscard]] Configuration* GetConfiguration() const { return configuration_; }
  [[nodiscard]] DataHeader* GetDataHeader() const { return data_header_; }
  [[nodiscard]] const std::map<std::string, BranchPointer>& GetBranchPointers() const { return branches_; }
  [[nodiscard]] const std::map<std::string, Matching*>& GetMatchPointers() const { return matches_; }

  BranchPointer GetPointerToBranch(const std::string& name){
    auto br = branches_.find(name);
    if(br != branches_.end()){
      return br->second;
    }
    else{
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

  Long64_t Draw(const char *varexp, const char *selection = nullptr, Option_t *option = "", Long64_t nentries = kMaxEntries, Long64_t firstentry = 0) override;

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

  Configuration* configuration_;
  DataHeader* data_header_;

  std::map<std::string, BranchPointer> branches_{};
  std::map<std::string, Matching*> matches_{};

  ClassDefOverride(AnalysisTree::Chain, 1)

};

}
#endif//ANALYSISTREE_INFRA_CHAIN_HPP_
