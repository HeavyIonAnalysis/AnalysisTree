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

  void Print() const;

  void SetSystem(std::string sys) { system_ = std::move(sys); }
  void SetBeamMomentum(float mom, float m_target = 0.938, float m_beam = 0.938);
  void SetDetectorPosition(const TVector3& pos) {
    det_pos_.emplace_back(pos);
  }

  [[nodiscard]] Floating_t GetBeamRapidity() const { return beam_y_; }
  [[nodiscard]] std::string GetSystem() const { return system_; }
  [[nodiscard]] const TVector3& GetDetectorPosition(int i) const { return det_pos_.at(i); }

  [[nodiscard]] const ModulePositions& GetModulePositions(Integer_t idet) {
    return modules_pos_.at(idet);
  }

  [[nodiscard]] Floating_t GetModulePhi(int det_id, int module_id) const {
    return modules_pos_.at(det_id).GetChannel(module_id).GetPhi();
  }

 protected:
  std::vector<ModulePositions> modules_pos_;///< in a local coordinate system
  std::vector<TVector3> det_pos_{};

  std::string system_;
  Floating_t beam_mom_{UndefValueFloat};
  Floating_t beam_y_{UndefValueFloat};
  Floating_t sqrtsNN_{UndefValueFloat};

  ClassDef(AnalysisTree::DataHeader, 1)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_DATAHEADER_H
