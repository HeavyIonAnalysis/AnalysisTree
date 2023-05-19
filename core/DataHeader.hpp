/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_DATAHEADER_H
#define ANALYSISTREE_DATAHEADER_H

#include <vector>

#include "TObject.h"
#include "TVector3.h"

#include "Detector.hpp"

namespace AnalysisTree {

/**
 * A class for a static info
 */
class DataHeader : public TObject {
 public:
  DataHeader() = default;
  DataHeader(const DataHeader&) = default;
  DataHeader(DataHeader&&) = default;
  DataHeader& operator=(DataHeader&&) = default;
  DataHeader& operator=(const DataHeader&) = default;

  ModulePositions& AddDetector();

  void Print(Option_t* = "") const;

  void SetSystem(std::string sys) { system_ = std::move(sys); }
  void SetBeamMomentum(float mom, float m_target = 0.938, float m_beam = 0.938);
  void SetDetectorPosition(const TVector3& pos) {
    det_pos_.emplace_back(pos);
  }
  void SetTimeSliceLength(float tslength) { ts_length_ = tslength; }

  ANALYSISTREE_ATTR_NODISCARD Floating_t GetBeamRapidity() const { return beam_y_; }
  ANALYSISTREE_ATTR_NODISCARD std::string GetSystem() const { return system_; }
  ANALYSISTREE_ATTR_NODISCARD const TVector3& GetDetectorPosition(int i) const { return det_pos_.at(i); }
  ANALYSISTREE_ATTR_NODISCARD Floating_t GetTimeSliceLength() const { return ts_length_; }

  ANALYSISTREE_ATTR_NODISCARD const ModulePositions& GetModulePositions(Integer_t idet) const {
    return modules_pos_.at(idet);
  }

  ANALYSISTREE_ATTR_NODISCARD Floating_t GetModulePhi(int det_id, int module_id) const {
    return modules_pos_.at(det_id).GetChannel(module_id).GetPhi();
  }

 protected:
  std::vector<ModulePositions> modules_pos_;///< in a local coordinate system
  std::vector<TVector3> det_pos_{};

  std::string system_;
  Floating_t beam_mom_{UndefValueFloat};
  Floating_t beam_y_{UndefValueFloat};
  Floating_t sqrtsNN_{UndefValueFloat};
  Floating_t ts_length_{UndefValueFloat};

  ClassDef(DataHeader, 1)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_DATAHEADER_H
