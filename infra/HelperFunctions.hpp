#ifndef ANALYSISTREE_INFRA_HELPER_FUNCTIONS_HPP
#define ANALYSISTREE_INFRA_HELPER_FUNCTIONS_HPP

#include "SimpleCut.hpp"

#include <string>
#include <vector>

namespace HelperFunctions {

template<typename T>
inline std::string ToStringWithPrecision(const T a_value, const int n) {
  std::ostringstream out;
  out.precision(n);
  out << std::fixed << a_value;
  return out.str();
}

inline std::vector<AnalysisTree::SimpleCut> CreateSliceCuts(const std::vector<float>& ranges, const std::string& cutNamePrefix, const std::string& branchFieldName) {
  std::vector<AnalysisTree::SimpleCut> sliceCuts;
  for (int iRange = 0; iRange < ranges.size() - 1; iRange++) {
    const std::string cutName = cutNamePrefix + ToStringWithPrecision(ranges.at(iRange), 2) + "_" + ToStringWithPrecision(ranges.at(iRange + 1), 2);
    sliceCuts.emplace_back(AnalysisTree::RangeCut(branchFieldName, ranges.at(iRange), ranges.at(iRange + 1), cutName));
  }

  return sliceCuts;
}

}// namespace HelperFunctions
#endif// ANALYSISTREE_INFRA_HELPER_FUNCTIONS_HPP
