#ifndef ANALYSISTREE_GENERICTRACK_H
#define ANALYSISTREE_GENERICTRACK_H

#include <cmath>
#include <iostream>

#include <TLorentzVector.h>
#include <TVector3.h>

#include "Container.hpp"
#include "Constants.hpp"

namespace AnalysisTree {

/**
 * A class for a generic track with determined momentum and charge
 */

class Track : public Container {
 public:
  Track() = default;

  explicit Track(Integer_t id) : Container(id) {}
  Track(const Track &track) = default;
  Track(Track &&track) = default;
  Track& operator=(Track&&) = default;
  Track& operator= (const Track &track) = default;

  /**
   * This is a typical semantics for the getter
   * of physical vector and this semantics should be followed by anyone
   * @return 4d-momentum of track
   */
  inline TLorentzVector Get4MomentumByMass(Floating_t mass) const {
    return TLorentzVector({px_, py_, pz_}, sqrt( px_*px_ + py_*py_ + pz_*pz_ + mass*mass )) ;
  }

  inline TLorentzVector Get4Momentum(PdgCode_t pid) const {
    const float mass = GetMassByPdgId(pid);
    return Get4MomentumByMass(mass);
  }

  void SetMomentum(Floating_t px, Floating_t py, Floating_t pz) {
    px_ = px;
    py_ = py;
    pz_ = pz;
  }
  
  /**
   * @return 3d-momentum of a track
   */

  void SetMomentum3(const TVector3 &momentum) {
    px_ = momentum.Px();
    py_ = momentum.Py();
    pz_ = momentum.Pz();
  }

  inline Floating_t GetPx() const { return px_; }
  inline Floating_t GetPy() const { return py_; }
  inline Floating_t GetPz() const { return pz_; }
  inline Floating_t GetPt() const  { return sqrt(px_*px_ + py_*py_); }
  inline Floating_t GetPhi() const  { return atan2(py_, px_); }
  inline Floating_t GetEta() const  { return 0.5 * log ( (GetP() + pz_) / (GetP()-pz_) ); }
  inline Floating_t GetP() const  { return sqrt(px_*px_ + py_*py_ + pz_*pz_); }
  inline TVector3 GetMomentum3() const { return TVector3(px_, py_, pz_); }

  friend bool operator==(const Track &that, const Track &other);


  Floating_t GetRapidity(PdgCode_t pid) const {
    const float mass = GetMassByPdgId(pid);
    return GetRapidityByMass(mass);
  }

  Floating_t GetRapidityByMass(float mass) const  {
    const float e = sqrt( mass*mass + GetP()*GetP() );
    return 0.5*log( (e+GetPz()) / (e-GetPz()) );
  }

  template<typename T>
  T GetField(Integer_t iField) const {
    if (iField >= 0)
      return Container::GetField<T>(iField);  
    else{              //TODO fix for T=int
      switch (iField) {
        case TrackFields::kPhi : return GetPhi();
        case TrackFields::kPt : return GetPt();
        case TrackFields::kEta : return GetEta();
        case TrackFields::kP : return GetP();
        case TrackFields::kPx : return GetPx();
        case TrackFields::kPy : return GetPy();
        case TrackFields::kPz : return GetPz();
        default : throw std::out_of_range("Track::GetField - Index " + std::to_string(iField) + " is not found");
      }
    }
  }

  void Print() const;

protected:

  Floating_t px_{UndefValueFloat}; ///< x-component of track's momentum
  Floating_t py_{UndefValueFloat}; ///< y-component of track's momentum
  Floating_t pz_{UndefValueFloat}; ///< z-component of track's momentum

  ClassDef(AnalysisTree::Track, 1);
};

}
#endif //ANALYSISTREE_GENERICTRACK_H
