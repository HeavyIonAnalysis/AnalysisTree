#ifndef ANALYSISTREEQA_SRC_BRANCHREADER_H_
#define ANALYSISTREEQA_SRC_BRANCHREADER_H_

#include <utility>
#include <string>
#include <vector>
#include <variant>// NOTE

#include "Detector.hpp"
#include "EventHeader.hpp"
#include "Constants.hpp"
#include "Variable.hpp"

namespace AnalysisTree {

class Variable;
class Cuts;

#if USEBOOST
using BranchPointer = boost::variant<TrackDetector *, Particles *, ModuleDetector *, HitDetector *, EventHeader *>;
#else
using BranchPointer = std::variant<TrackDetector *, Particles *, ModuleDetector *, HitDetector *, EventHeader *>;
#endif

class BranchReader {

 public:
  BranchReader() = default;
  BranchReader(std::string  name, void *data, DetType type, Cuts *cuts = nullptr);

  const std::string &GetName() const { return name_; }
  DetType GetType() const { return type_; }
  const Cuts *GetCut() const { return cuts_; }

  size_t GetNumberOfChannels();
  bool ApplyCut(int i_channel);

  double GetValue(Variable var, int i_channel){
    return std::visit([var, i_channel](auto &&arg) { return var.GetValue(arg->GetChannel(i_channel)); }, data_);
  }

  const BranchPointer& GetData() const {
    return data_;
  }

  int GetId() const { return id_; }

 protected:

  std::string name_{""};
  BranchPointer data_{};
  Cuts *cuts_{nullptr};
  int id_{-1};
  DetType type_{DetType(-1)};
};

}// namespace AnalysisTree

#endif//ANALYSISTREEQA_SRC_BRANCHREADER_H_
