#ifndef ANALYSISTREE_INFRA_HELPER_FUNCTIONS_HPP
#define ANALYSISTREE_INFRA_HELPER_FUNCTIONS_HPP

#include "SimpleCut.hpp"

#include <TFile.h>
#include <TH1.h>

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

inline std::vector<AnalysisTree::SimpleCut> CreateRangeCuts(const std::vector<float>& ranges, const std::string& cutNamePrefix, const std::string& branchFieldName, bool isAppendWithOpenCut = false, int precision = -1) {
  auto checkHasFractionalPart = [](float value) {
    float fracPart = std::round(value) - value;
    return std::abs(fracPart) > 1e-4;
  };

  auto countDigisAfterComma = [&](float value) {
    int nDigis{0};
    while (checkHasFractionalPart(value)) {
      value *= 10;
      ++nDigis;
    }

    return nDigis;
  };

  auto evaluateMaxDigisAfterComma = [&](const std::vector<float>& vec) {
    int result = 0;
    for (const auto& v : vec) {
      result = std::max(result, countDigisAfterComma(v));
    }

    return result;
  };

  if (precision < 0) precision = evaluateMaxDigisAfterComma(ranges);

  std::vector<AnalysisTree::SimpleCut> sliceCuts;
  for (int iRange = 0; iRange < static_cast<int>(ranges.size()) - 1; iRange++) {
    const std::string cutName = cutNamePrefix + ToStringWithPrecision(ranges.at(iRange), precision) + "_" + ToStringWithPrecision(ranges.at(iRange + 1), precision);
    sliceCuts.emplace_back(AnalysisTree::RangeCut(branchFieldName, ranges.at(iRange), ranges.at(iRange + 1), cutName));
  }

  if (isAppendWithOpenCut) sliceCuts.emplace_back(AnalysisTree::OpenCut(branchFieldName.substr(0, branchFieldName.find('.'))));

  return sliceCuts;
}

inline std::vector<AnalysisTree::SimpleCut> CreateEqualCuts(const std::vector<int>& values, const std::string& cutNamePrefix, const std::string& branchFieldName) {
  std::vector<AnalysisTree::SimpleCut> sliceCuts;
  for (const auto& value : values) {
    const std::string cutName = cutNamePrefix + std::to_string(value);
    sliceCuts.emplace_back(AnalysisTree::EqualsCut(branchFieldName, value, cutName));
  }

  return sliceCuts;
}

inline bool StringToBool(const std::string& str) {
  if (str == "true") return true;
  else if (str == "false")
    return false;
  else
    throw std::runtime_error("HelperFunctions::StringToBool(): argument must be either true or false");
}

template<typename T>
inline std::vector<T> MergeVectors(const std::vector<T>& vec1, const std::vector<T>& vec2) {
  std::vector<T> result;
  result.reserve(vec1.size() + vec2.size());
  result.insert(result.end(), vec1.begin(), vec1.end());
  result.insert(result.end(), vec2.begin(), vec2.end());

  return result;
}

template<typename T, typename... Args>
inline std::vector<T> MergeVectors(const std::vector<T>& vec1, const std::vector<T>& vec2, const Args&... args) {
  return MergeVectors(vec1, MergeVectors(vec2, args...));
}

inline TFile* OpenFileWithNullptrCheck(const std::string& fileName, const std::string& option = "read") {
  TFile* file = TFile::Open(fileName.c_str(), option.c_str());
  if (file == nullptr) {
    throw std::runtime_error("HelperFunctions::OpenFileWithNullptrCheck() - file " + fileName + " is missing");
  }
  return file;
}

template<typename T>
inline T* GetObjectWithNullptrCheck(TFile* fileIn, const std::string& objectName) {
  T* ptr = fileIn->Get<T>(objectName.c_str());
  if (ptr == nullptr) {
    throw std::runtime_error("HelperFunctions::GetObjectWithNullptrCheck() - object " + objectName + " in file " + fileIn->GetName() + " is missing");
  }
  return ptr;
}

inline void CheckHistogramsForXaxisIdentity(const TH1* h1, const TH1* h2) {
  if (h1->GetNbinsX() != h2->GetNbinsX()) {
    throw std::runtime_error("HelperFunctions::CheckHistogramsForXaxisIdentity(): nBinsX do not match for " + static_cast<std::string>(h1->GetName()) + " and " + h2->GetName());
  }
  const int nBins = h1->GetNbinsX();
  for (int iBin = 1; iBin <= nBins; iBin++) {
    if (std::abs(h1->GetBinCenter(iBin) - h2->GetBinCenter(iBin)) > 1e-6) {
      throw std::runtime_error("HelperFunctions::CheckHistogramsForXaxisIdentity(): bins do not coincide for " + static_cast<std::string>(h1->GetName()) + " and " + h2->GetName());
    }
  }
}

inline void Sumw2IfNotYet(TH1* histo, bool value = true) {
  const bool isSumw2Already = histo->GetSumw2N() > 0;
  if (isSumw2Already != value) histo->Sumw2(value);
}

inline TH1* MergeHistograms(const std::vector<TH1*>& histos) {
  bool isSumw2{false};
  for(const auto& h : histos) {
    CheckHistogramsForXaxisIdentity(h, histos.at(0));
    isSumw2 |= h->GetSumw2N() > 0;
  }

  TH1* hResult = dynamic_cast<TH1*>(histos.at(0)->Clone("hMerged"));
  Sumw2IfNotYet(hResult);
  hResult->SetDirectory(nullptr);
  for(size_t iH = 1, nHs = histos.size(); iH < nHs; ++iH) {
    hResult->Add(histos.at(iH));
  }
  Sumw2IfNotYet(hResult, isSumw2);

  return hResult;
}

inline TH1* MergeHistograms(TFile* fileIn, const std::vector<std::string>& histoNames) {
  std::vector<TH1*> histos;
  for (const auto& hN : histoNames) {
    histos.emplace_back(GetObjectWithNullptrCheck<TH1>(fileIn, hN));
  }
  TH1* hResult = MergeHistograms(histos);

  return hResult;
}

template<typename T>
inline TDirectory* MkDirIfNotExists(T* fileOrDirectory, const std::string& name) {
  if (fileOrDirectory == nullptr) throw std::runtime_error("HelperFunctions::MkDirIfNotExists(): file or directory ptr is null");
  TDirectory* result = fileOrDirectory->GetDirectory(name.c_str());
  if (result == nullptr) fileOrDirectory->mkdir(name.c_str());
  result = fileOrDirectory->GetDirectory(name.c_str());
  return result;
}

template<typename T>
inline void CD(T* fileOrDirectory, const std::string& name) {
  auto destination = MkDirIfNotExists(fileOrDirectory, name);
  destination->cd();
}

inline double InterpolateTH1SuppressWarning(const TH1* h, double value) {
  double result;
  if (value <= h->GetBinLowEdge(1) || value >= h->GetBinLowEdge(h->GetNbinsX() + 1)) result = 0.;
  else
    result = h->Interpolate(value);
  return result;
}

}// namespace HelperFunctions
#endif// ANALYSISTREE_INFRA_HELPER_FUNCTIONS_HPP
