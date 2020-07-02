#ifndef ANALYSISTREEQA_SRC_BRANCHREADER_H_
#define ANALYSISTREEQA_SRC_BRANCHREADER_H_

#include <utility>
#include <variant>// NOTE

#include "Cuts.hpp"
#include "Detector.hpp"
#include "EventHeader.hpp"
#include "Variable.hpp"

namespace AnalysisTree {

#if USEBOOST
using BranchPointer = boost::variant<TrackDetector *, Particles *, ModuleDetector *, HitDetector *, EventHeader *>;
#else
using BranchPointer = std::variant<TrackDetector *, Particles *, ModuleDetector *, HitDetector *, EventHeader *>;
#endif

class BranchReader {

 public:
  BranchReader() = default;
  BranchReader(std::string name, void *data, DetType type, Cuts *cuts = nullptr);

  void FillValues();
  void AddVariable(const Variable &var) { vars_.emplace_back(var); }

  const std::string &GetName() const { return name_; }
  DetType GetType() const { return type_; }
  const Cuts *GetCut() const { return cuts_; }

  const std::vector<Variable> &GetVariables() const { return vars_; }
  std::vector<Variable> &Variables() { return vars_; }

  const std::vector<double> &GetValues(int i_var) const { return values_.at(i_var); }

 protected:
  size_t GetNumberOfChannels();
  bool ApplyCut(int i_channel);
  void ClearAndReserveOutput(int n_channels);

  std::string name_{""};
  BranchPointer data_{};
  std::vector<Variable> vars_{};
  Cuts *cuts_{nullptr};

  std::vector<std::vector<double>> values_{};
  DetType type_{DetType(-1)};
};

}// namespace AnalysisTree

#endif//ANALYSISTREEQA_SRC_BRANCHREADER_H_
