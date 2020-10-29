#ifndef ANALYSISTREE_INFRA_CHAIN_HPP_
#define ANALYSISTREE_INFRA_CHAIN_HPP_

#include <TFile.h>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "Configuration.hpp"
#include "DataHeader.hpp"

class TChain;

namespace AnalysisTree{

class Chain {

 public:

  Chain() = delete;
  Chain(const std::vector<std::string>& filelists, const std::vector<std::string>& treenames) {
    InitChain(filelists, treenames);
    InitConfiguration();
    InitDataHeader();
  };

  [[nodiscard]] const Configuration& GetConfiguration() const { return configuration_; }
  [[nodiscard]] const DataHeader& GetDataHeader() const { return data_header_; }
  [[nodiscard]] const std::map<std::string, void*>& GetBranchPointers() const { return branches_; }

  void* GetPointerToBranch(const std::string& name){
    auto br = branches_.find(name);
    if(br != branches_.end()){
      return br->second;
    }
    else{
      throw std::runtime_error("Branch " + name + " is not found!");
    }
  }

 protected:

  void InitChain(const std::vector<std::string>& filelists, const std::vector<std::string>& treenames);
  void InitPointersToBranches(std::set<std::string> names);
  void InitConfiguration();
  void InitDataHeader();

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

  std::vector<std::string> filelists_{};
  std::vector<std::string> treenames_{};
  TChain* chain_{nullptr};

  Configuration configuration_;
  DataHeader data_header_;

  std::map<std::string, void*> branches_{};

};

}
#endif//ANALYSISTREE_INFRA_CHAIN_HPP_
