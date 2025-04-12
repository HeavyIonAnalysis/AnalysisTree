#ifndef ANALYSISTREE_INFRA_HELPER_FUNCTIONS_HPP
#define ANALYSISTREE_INFRA_HELPER_FUNCTIONS_HPP

#include "SimpleCut.hpp"

#include <sstream>
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

template<typename T>
inline std::string ToStringWithSignificantFigures(const T a_value, const int n) {
  if (a_value == 0) return "0";

  const double dMag = std::log10(std::abs(a_value));// scale of the a_value (e.g 1.* for 1.2345, 2.* for 12.345 etc)
  const int iMag = static_cast<int>(dMag - n + 1 > 0 ? dMag - n + 1 : dMag - n);
  const T shifted_value = a_value / std::pow(10, iMag);             // shift decimal point to have all required digits to l.h.s. from it
  const T rounded_value = static_cast<T>(std::round(shifted_value));// get rid of r.h.s. from decimal point
  const T reshifted_value = rounded_value * std::pow(10, iMag);     // return decimal point to its original place
  const int precision = iMag < 0 ? -iMag : 0;                       // determine how many digits after decimal point one needs
  return ToStringWithPrecision(reshifted_value, precision);
}

inline std::vector<AnalysisTree::SimpleCut> CreateRangeCuts(const std::vector<float>& ranges, const std::string& cutNamePrefix, const std::string& branchFieldName, int precision = 2) {
  std::vector<AnalysisTree::SimpleCut> sliceCuts;
  for (int iRange = 0; iRange < ranges.size() - 1; iRange++) {
    const std::string cutName = cutNamePrefix + ToStringWithPrecision(ranges.at(iRange), precision) + "_" + ToStringWithPrecision(ranges.at(iRange + 1), precision);
    sliceCuts.emplace_back(AnalysisTree::RangeCut(branchFieldName, ranges.at(iRange), ranges.at(iRange + 1), cutName));
  }

  return sliceCuts;
}

inline std::vector<AnalysisTree::SimpleCut> CreateEqualCuts(const std::vector<float>& values, const std::string& cutNamePrefix, const std::string& branchFieldName, int precision = 2) {
  std::vector<AnalysisTree::SimpleCut> sliceCuts;
  for (int iValue = 0; iValue < values.size(); iValue++) {
    const std::string cutName = cutNamePrefix + ToStringWithPrecision(values.at(iValue), precision);
    sliceCuts.emplace_back(AnalysisTree::EqualsCut(branchFieldName, values.at(iValue), cutName));
  }

  return sliceCuts;
}

inline bool StringToBool(const std::string& str) {
  if(str == "true") return true;
  else if(str == "false") return false;
  else throw std::runtime_error("HelperFunctions::StringToBool(): argument must be either true or false");
}

}// namespace HelperFunctions
#endif// ANALYSISTREE_INFRA_HELPER_FUNCTIONS_HPP
