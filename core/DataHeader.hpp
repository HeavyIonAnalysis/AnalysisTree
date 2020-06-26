#ifndef ANALYSISTREE_DATAHEADER_H
#define ANALYSISTREE_DATAHEADER_H

#include <utility>
#include <vector>
#include <array>
#include <cmath>

#include "TObject.h"
#include "TVector3.h"

#include "Constants.hpp"
#include "Detector.hpp"

namespace AnalysisTree{

/**
 * A class for a static info
 */
class DataHeader : public TObject {
 public:

  DataHeader() = default;
  DataHeader(const DataHeader&) = default;
  DataHeader(DataHeader&&) = default;
  DataHeader& operator=(DataHeader&&) = default;
  DataHeader& operator= (const DataHeader &) = default;

  void SetSystem(std::string sys) {system_ = std::move(sys);}
  void SetBeamMomentum(float mom, float m_target=0.938, float m_beam=0.938) {
    beam_mom_ = mom;
    const float energy_lab=sqrt(m_beam*m_beam + beam_mom_*beam_mom_);
    sqrtsNN_=sqrt( m_target*m_target + m_beam*m_beam +2*m_target*energy_lab);
    const float E = sqrtsNN_/2;
    const float p = sqrt(E*E - m_target*m_target);
    beam_y_ = log((E+p)/(E-p))/2.;
  }

  Floating_t GetBeamRapidity() const { return beam_y_; }
  std::string GetSystem() const { return system_; }

  ModulePositions& AddDetector(){
    modules_pos_.emplace_back(modules_pos_.size() );
    return modules_pos_.back();
  }
  
  const ModulePositions& GetModulePositions(Integer_t idet){
    return modules_pos_.at(idet);
  }
  
  void SetDetectorPosition(const TVector3& pos){
    det_pos_.emplace_back( pos );
  }

  const TVector3& GetDetectorPosition(int i) const { return det_pos_.at(i); }

  void Print() const
  {
    std::cout << "AnalysisTree::DataHeader::Print()" << std::endl;
    std::cout << " Collision system is " << system_ << std::endl;
    std::cout << " at beam momentum " << beam_mom_;
    std::cout << " (sqrt(s_NN) = " << sqrtsNN_ << ")" << std::endl; 
    std::cout << " beam rapidity is " << beam_y_ << std::endl; 
    
    for (const auto& mod_pos : modules_pos_){
      mod_pos.Print();
    }
  }
  
  Floating_t GetModulePhi(int det_id, int module_id) const {
    return modules_pos_.at(det_id).GetChannel(module_id).GetPhi();
  }
  
 protected:
  std::vector<ModulePositions> modules_pos_; ///< in a local coordinate system
  std::vector<TVector3> det_pos_{};

  std::string system_{""};
  Floating_t beam_mom_{UndefValueFloat};
  Floating_t beam_y_{UndefValueFloat};
  Floating_t sqrtsNN_{UndefValueFloat};

  ClassDef(AnalysisTree::DataHeader, 1)

  
};


}
#endif //ANALYSISTREE_DATAHEADER_H
