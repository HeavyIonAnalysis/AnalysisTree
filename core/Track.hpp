/* Copyright (C) 2019-2021 GSI, Universität Tübingen, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Eugeny Kashirin, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_GENERICTRACK_H
#define ANALYSISTREE_GENERICTRACK_H

#include <cmath>
#include <stdexcept>

//#include <Math/Vector4D.h>
#include <TVector3.h>

#include "Container.hpp"

namespace AnalysisTree {

/*! \brief A class for a generic track with determined momentum
 *
 *  Detailed description starts here...
 */

class Track : public Container {
 public:
  Track() = default;

  explicit Track(size_t id) noexcept : Container(id) {}
  explicit Track(const Container& cont) : Container(cont) {}
  Track(size_t id, const BranchConfig& branch) noexcept : Container(id, branch) {}
  Track(const Track& track) = default;
  Track(Track&& track) = default;
  Track& operator=(Track&&) = default;
  Track& operator=(const Track& track) = default;
  ~Track() override = default;

  /**
  * Calculates 4-momentum
  * @param mass - track mass hypotesis in GeV/c^2
  * @return 4d-momentum of track
  */
  //  ANALYSISTREE_ATTR_NODISCARD inline ROOT::Math::PxPyPzMVector Get4MomentumByMass(Floating_t mass) const noexcept {
  //    return {px_, py_, pz_, mass};
  //  }

  /**
  * Calculates 4-momentum
  * @param pdg - pdg code hypotesis
  * @return 4d-momentum of track. If pdg is not found - exeption should be thrown.
  */
  //  ANALYSISTREE_ATTR_NODISCARD inline ROOT::Math::PxPyPzMVector Get4Momentum(PdgCode_t pdg) const {
  //    const float mass = GetMassByPdgId(pdg);
  //    return Get4MomentumByMass(mass);
  //  }

  void SetMomentum(Floating_t px, Floating_t py, Floating_t pz) noexcept {
    px_ = px;
    py_ = py;
    pz_ = pz;
  }

  void SetMomentum3(const TVector3& momentum) noexcept {
    px_ = momentum.Px();
    py_ = momentum.Py();
    pz_ = momentum.Pz();
  }

  ANALYSISTREE_ATTR_NODISCARD inline Floating_t GetPx() const noexcept { return px_; }
  ANALYSISTREE_ATTR_NODISCARD inline Floating_t GetPy() const noexcept { return py_; }
  ANALYSISTREE_ATTR_NODISCARD inline Floating_t GetPz() const noexcept { return pz_; }
  ANALYSISTREE_ATTR_NODISCARD inline Floating_t GetPt() const noexcept { return sqrt(px_ * px_ + py_ * py_); }
  ANALYSISTREE_ATTR_NODISCARD inline Floating_t GetPhi() const noexcept { return atan2(py_, px_); }
  ANALYSISTREE_ATTR_NODISCARD inline Floating_t GetEta() const noexcept { return 0.5f * log((GetP() + pz_) / (GetP() - pz_)); }
  ANALYSISTREE_ATTR_NODISCARD inline Floating_t GetP() const noexcept { return sqrt(px_ * px_ + py_ * py_ + pz_ * pz_); }

  /**
  * @return 3d-momentum of a track
  */
  ANALYSISTREE_ATTR_NODISCARD inline TVector3 GetMomentum3() const noexcept { return {px_, py_, pz_}; }

  /**
  * Compares 2 tracks
  * @param that, other - tracks to compare
  */
  friend bool operator==(const Track& that, const Track& other) noexcept;

  /**
  * Calculates rapidity
  * @param pdg - pdg code hypotesis
  * @return rapidity of the track. If pdg is not found - exeption should be thrown.
  */
  ANALYSISTREE_ATTR_NODISCARD Floating_t GetRapidity(PdgCode_t pdg) const {
    const float mass = GetMassByPdgId(pdg);
    return GetRapidityByMass(mass);
  }

  /**
  * Calculates rapidity
  * @param mass - track mass hypotesis in GeV/c^2
  * @return rapidity of the track
  */
  ANALYSISTREE_ATTR_NODISCARD Floating_t GetRapidityByMass(float mass) const noexcept {
    const float e = sqrt(mass * mass + GetP() * GetP());
    return 0.5f * log((e + GetPz()) / (e - GetPz()));
  }

  /**
  * Calculates field
  * @param id - number of the field. In case of negative values corresponding pre-defined field will be calculated
  * @return field value. In case it is not found exeption is thrown
  */
  template<typename T>
  T GetField(Integer_t id) const {
    if (id >= 0)
      return Container::GetField<T>(id);
    else {//TODO fix for T=int
      switch (id) {
        case TrackFields::kPhi: return GetPhi();
        case TrackFields::kPt: return GetPt();
        case TrackFields::kEta: return GetEta();
        case TrackFields::kP: return GetP();
        case TrackFields::kPx: return GetPx();
        case TrackFields::kPy: return GetPy();
        case TrackFields::kPz: return GetPz();
        case TrackFields::kId: return GetId();
        default: throw std::out_of_range("Track::GetField - Index " + std::to_string(id) + " is not found");
      }
    }
  }

  template<typename T>
  void SetField(T value, Int_t field_id) {
    if (field_id >= 0) {
      Container::SetField(value, field_id);
    } else {
      switch (field_id) {
        case TrackFields::kPx: px_ = value; break;
        case TrackFields::kPy: py_ = value; break;
        case TrackFields::kPz: pz_ = value; break;
        case TrackFields::kId: break;
        case TrackFields::kP: /*throw std::runtime_error("Cannot set transient fields");*/ break;
        case TrackFields::kPt: /*throw std::runtime_error("Cannot set transient fields");*/ break;
        case TrackFields::kEta: /*throw std::runtime_error("Cannot set transient fields");*/ break;
        case TrackFields::kPhi: /*throw std::runtime_error("Cannot set transient fields");*/ break;
        default: throw std::runtime_error("Unknown field");
      }
    }
  }

  /**
  * Prints the track content
  */
  void Print() const noexcept override;

 protected:
  static float GetMassByPdgId(PdgCode_t pdg);

  Floating_t px_{UndefValueFloat};///< x-component of track's momentum
  Floating_t py_{UndefValueFloat};///< y-component of track's momentum
  Floating_t pz_{UndefValueFloat};///< z-component of track's momentum

  ClassDefOverride(Track, 2);
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_GENERICTRACK_H
