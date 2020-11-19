#ifndef ANALYSISTREE_GENERICTRACK_H
#define ANALYSISTREE_GENERICTRACK_H

#include <cmath>

#include <TLorentzVector.h>
#include <TVector3.h>

#include "Container.hpp"

namespace AnalysisTree {

/*! \brief A class for a generic track with determined momentum
 *
 *  Detailed description starts here...
 */

class Track : public Container {
 public:
  Track() noexcept = default;

  explicit Track(Integer_t id) noexcept : Container(id) {}
  Track(const Track& track) = default;
  Track(Track&& track) noexcept = default;
  Track& operator=(Track&&) noexcept = default;
  Track& operator=(const Track& track) = default;
  ~Track() override = default;

  virtual ~Track() {};

  /**
  * Calculates 4-momentum
  * @param mass - track mass hypotesis in GeV/c^2
  * @return 4d-momentum of track
  */
  [[nodiscard]] inline TLorentzVector Get4MomentumByMass(Floating_t mass) const noexcept {
    return TLorentzVector({px_, py_, pz_}, sqrt(px_ * px_ + py_ * py_ + pz_ * pz_ + mass * mass));
  }

  /**
  * Calculates 4-momentum
  * @param pdg - pdg code hypotesis
  * @return 4d-momentum of track. If pdg is not found - exeption should be thrown.
  */
  [[nodiscard]] inline TLorentzVector Get4Momentum(PdgCode_t pdg) const {
    const float mass = GetMassByPdgId(pdg);
    return Get4MomentumByMass(mass);
  }

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

  [[nodiscard]] inline Floating_t GetPx() const noexcept { return px_; }
  [[nodiscard]] inline Floating_t GetPy() const noexcept { return py_; }
  [[nodiscard]] inline Floating_t GetPz() const noexcept { return pz_; }
  [[nodiscard]] inline Floating_t GetPt() const noexcept { return sqrt(px_ * px_ + py_ * py_); }
  [[nodiscard]] inline Floating_t GetPhi() const noexcept { return atan2(py_, px_); }
  [[nodiscard]] inline Floating_t GetEta() const noexcept { return 0.5 * log((GetP() + pz_) / (GetP() - pz_)); }
  [[nodiscard]] inline Floating_t GetP() const noexcept { return sqrt(px_ * px_ + py_ * py_ + pz_ * pz_); }

  /**
  * @return 3d-momentum of a track
  */
  [[nodiscard]] inline TVector3 GetMomentum3() const noexcept { return TVector3(px_, py_, pz_); }

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
  [[nodiscard]] Floating_t GetRapidity(PdgCode_t pdg) const {
    const float mass = GetMassByPdgId(pdg);
    return GetRapidityByMass(mass);
  }

  /**
  * Calculates rapidity
  * @param mass - track mass hypotesis in GeV/c^2
  * @return rapidity of the track
  */
  [[nodiscard]] Floating_t GetRapidityByMass(float mass) const noexcept {
    const float e = sqrt(mass * mass + GetP() * GetP());
    return 0.5 * log((e + GetPz()) / (e - GetPz()));
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
    else { //TODO fix for T=int
      switch (id) {
        case TrackFields::kPhi: return GetPhi();
        case TrackFields::kPt: return GetPt();
        case TrackFields::kEta: return GetEta();
        case TrackFields::kP: return GetP();
        case TrackFields::kPx: return GetPx();
        case TrackFields::kPy: return GetPy();
        case TrackFields::kPz: return GetPz();
        default: throw std::out_of_range("Track::GetField - Index " + std::to_string(id) + " is not found");
      }
    }
  }

  /**
  * Prints the track content
  */
  void Print() const noexcept;

 protected:

  static float GetMassByPdgId(PdgCode_t pdg);

  Floating_t px_{UndefValueFloat}; ///< x-component of track's momentum
  Floating_t py_{UndefValueFloat}; ///< y-component of track's momentum
  Floating_t pz_{UndefValueFloat}; ///< z-component of track's momentum

  ClassDefOverride(AnalysisTree::Track, 1);
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_GENERICTRACK_H
